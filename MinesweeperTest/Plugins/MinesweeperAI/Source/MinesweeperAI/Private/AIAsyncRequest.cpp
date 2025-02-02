// Fill out your copyright notice in the Description page of Project Settings.


#include "AIAsyncRequest.h"

#include "HttpModule.h"
#include "MinesweeperDeveloperSettings.h"
#include "Interfaces/IHttpResponse.h"

DEFINE_LOG_CATEGORY_STATIC(AI_AsyncRequestLog, Log, All);


UOpenAIRequest* UOpenAIRequest::OpenAIRequest(FOpenAiRequestData Request)
{
	UOpenAIRequest* OpenAIRequestNode = NewObject<UOpenAIRequest>();
	OpenAIRequestNode->AddToRoot();
	OpenAIRequestNode->OnOpenAIRequest(Request);
	return OpenAIRequestNode;
}

void UOpenAIRequest::OnOpenAIRequest(FOpenAiRequestData Request)
{
	FString OpenAiApiKey = FString("proba");
	/*auto* DeveloperSettings = GetDefault<UMinesweeperDeveloperSettings>();
	if (DeveloperSettings)
	{
		OpenAiApiKey = DeveloperSettings->OpenAi_API_Key;
		if (OpenAiApiKey.IsEmpty())
		{
			UE_LOG(AI_AsyncRequestLog, Error, TEXT("Missing OpenAI API key in project settings. Please set it in Project Settings -> Plugins -> MinesweeperAI -> OpenAI API Key."));
			return;
		}
	}*/

	// Define the system message as an FString
	FString SystemMessage = TEXT("You are an AI that generates Minesweeper grids in JSON format.");
	SystemMessage += TEXT(" Always return a valid JSON object containing a 'grid' key.");
	SystemMessage += TEXT(" The value of 'grid' must be a 2D array where:");
	SystemMessage += TEXT(" - 'X' represents a mine.");
	SystemMessage += TEXT(" - Numbers represent the count of adjacent mines.");
	SystemMessage += TEXT(" Always generate unique patterns and never repeat previous grids.");
	SystemMessage += TEXT(" Your response must contain only the JSON output, nothing else.");

	// Construct the API request
	FString APIUrl = TEXT("https://api.openai.com/v1/chat/completions"); // Adjust if needed

	// Create JSON request payload
	TSharedPtr<FJsonObject> RequestPayload = MakeShareable(new FJsonObject);
	RequestPayload->SetStringField(TEXT("model"), TEXT("gpt-4-turbo")); // Use the appropriate model
	RequestPayload->SetNumberField(TEXT("max_tokens"), 100);
    
	// Construct messages array
	TArray<TSharedPtr<FJsonValue>> Messages;

	// ✅ Renamed JSON object to avoid conflict
	TSharedPtr<FJsonObject> SystemMessageObject = MakeShareable(new FJsonObject);
	SystemMessageObject->SetStringField(TEXT("role"), TEXT("system"));
	SystemMessageObject->SetStringField(TEXT("content"), SystemMessage);
	Messages.Add(MakeShareable(new FJsonValueObject(SystemMessageObject)));

	// User prompt message (requests a specific grid)
	TSharedPtr<FJsonObject> UserMessage = MakeShareable(new FJsonObject);
	UserMessage->SetStringField(TEXT("role"), TEXT("user"));
	UserMessage->SetStringField(TEXT("content"), Request.Prombt);
	Messages.Add(MakeShareable(new FJsonValueObject(UserMessage)));

	RequestPayload->SetArrayField(TEXT("messages"), Messages);

	// Convert to JSON string
	FString RequestContent;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestContent);
	FJsonSerializer::Serialize(RequestPayload.ToSharedRef(), Writer);

	// Create HTTP request
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HTTPRequest = FHttpModule::Get().CreateRequest();
	HTTPRequest->SetURL(APIUrl);
	HTTPRequest->SetVerb(TEXT("POST"));
	HTTPRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HTTPRequest->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *OpenAiApiKey));
	HTTPRequest->SetContentAsString(RequestContent);
	HTTPRequest->OnProcessRequestComplete().BindUObject(this, &UOpenAIRequest::OnHttpResponse);

	HTTPRequest->ProcessRequest();
}

void UOpenAIRequest::OnHttpResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	FBaseResponseData ResponseData;
	ResponseData.Success = bWasSuccessful;
	ResponseData.Message = bWasSuccessful ? TEXT("Request successful.") : TEXT("Request failed.");
	ResponseData.Code = Response.IsValid() ? Response->GetResponseCode() : 0;

	FOpenAiResponseData OpenAiResponseData;
	if (bWasSuccessful && Response.IsValid())
	{
		//log response
		FString ResponseString = Response->GetContentAsString();
		UE_LOG(AI_AsyncRequestLog, Log, TEXT("OpenAI response: %s"), *ResponseString);

		// Parse JSON response
		TSharedPtr<FJsonObject> JsonResponse;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

		if (FJsonSerializer::Deserialize(Reader, JsonResponse) && JsonResponse.IsValid())
		{
			// Extract "choices" -> "message" -> "content"
			TArray<TSharedPtr<FJsonValue>> Choices = JsonResponse->GetArrayField(TEXT("choices"));
			if (Choices.Num() > 0)
			{
				FString GridJson;
				if (Choices[0]->AsObject()->TryGetStringField(TEXT("content"), GridJson))
				{
					// Parse the returned grid JSON
					//ParseMinesweeperGrid(GridJson);
					OpenAiResponseData.GridJson = GridJson;
				}
			}
		}
	}
	OnCallback.Broadcast(OpenAiResponseData, ResponseData);
}


