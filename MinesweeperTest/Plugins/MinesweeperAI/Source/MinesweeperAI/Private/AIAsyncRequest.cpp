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
	auto* DeveloperSettings = GetDefault<UMinesweeperDeveloperSettings>();
	if (DeveloperSettings)
	{
		CacheOpenAiAPIKey = DeveloperSettings->OpenAi_API_Key;
		if (CacheOpenAiAPIKey.IsEmpty())
		{
			UE_LOG(AI_AsyncRequestLog, Error, TEXT("Missing OpenAI API key in project settings. Please set it in Project Settings -> Plugins -> MinesweeperAI -> OpenAI API Key."));
			return;
		}
	}
	FString APIUrl = TEXT("https://api.openai.com/v1/chat/completions");

	TSharedPtr<FJsonObject> RequestPayload = MakeShareable(new FJsonObject);
	RequestPayload->SetStringField(TEXT("model"), TEXT("gpt-4o-mini"));
	RequestPayload->SetNumberField(TEXT("max_tokens"), 200);
    
	TArray<TSharedPtr<FJsonValue>> Messages;

	TSharedPtr<FJsonObject> SystemMessageObject = MakeShareable(new FJsonObject);
	SystemMessageObject->SetStringField(TEXT("role"), TEXT("system"));
	SystemMessageObject->SetStringField(TEXT("content"), TEXT("You can call functions when needed."));
	Messages.Add(MakeShareable(new FJsonValueObject(SystemMessageObject)));

	TSharedPtr<FJsonObject> UserMessage = MakeShareable(new FJsonObject);
	UserMessage->SetStringField(TEXT("role"), TEXT("user"));
	UserMessage->SetStringField(TEXT("content"), Request.Prombt);
	Messages.Add(MakeShareable(new FJsonValueObject(UserMessage)));

	RequestPayload->SetArrayField(TEXT("messages"), Messages);

	TArray<TSharedPtr<FJsonValue>> Functions;
	TSharedPtr<FJsonObject> FunctionSchema = MakeShareable(new FJsonObject);
	FunctionSchema->SetStringField(TEXT("name"), TEXT("generate_minesweeper_grid"));
	FunctionSchema->SetStringField(TEXT("description"), TEXT("Generates a Minesweeper grid."));

	TSharedPtr<FJsonObject> Parameters = MakeShareable(new FJsonObject);
	Parameters->SetStringField(TEXT("type"), TEXT("object"));
	TSharedPtr<FJsonObject> Properties = MakeShareable(new FJsonObject);
	TSharedPtr<FJsonObject> RowsProperty = MakeShareable(new FJsonObject);
	RowsProperty->SetStringField(TEXT("type"), TEXT("integer"));
	Properties->SetObjectField(TEXT("rows"), RowsProperty);

	TSharedPtr<FJsonObject> ColsProperty = MakeShareable(new FJsonObject);
	ColsProperty->SetStringField(TEXT("type"), TEXT("integer"));
	Properties->SetObjectField(TEXT("cols"), ColsProperty);

	TSharedPtr<FJsonObject> MinesProperty = MakeShareable(new FJsonObject);
	MinesProperty->SetStringField(TEXT("type"), TEXT("integer"));
	Properties->SetObjectField(TEXT("mines"), MinesProperty);

	Parameters->SetObjectField(TEXT("properties"), Properties);
	Parameters->SetArrayField(TEXT("required"), { 
		MakeShareable(new FJsonValueString("rows")), 
		MakeShareable(new FJsonValueString("cols")), 
		MakeShareable(new FJsonValueString("mines")) 
	});

	FunctionSchema->SetObjectField(TEXT("parameters"), Parameters);
	Functions.Add(MakeShareable(new FJsonValueObject(FunctionSchema)));
	RequestPayload->SetArrayField(TEXT("functions"), Functions);
	RequestPayload->SetStringField(TEXT("function_call"), TEXT("auto")); // Auto-invoke the function


	FString RequestContent;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestContent);
	FJsonSerializer::Serialize(RequestPayload.ToSharedRef(), Writer);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HTTPRequest = FHttpModule::Get().CreateRequest();
	HTTPRequest->SetURL(APIUrl);
	HTTPRequest->SetVerb(TEXT("POST"));
	HTTPRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HTTPRequest->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *CacheOpenAiAPIKey));
	HTTPRequest->SetContentAsString(RequestContent);
	HTTPRequest->OnProcessRequestComplete().BindUObject(this, &UOpenAIRequest::OnHttpResponse);

	HTTPRequest->ProcessRequest();
}

void UOpenAIRequest::OnHttpResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{	
	if (bWasSuccessful && Response.IsValid())
	{
		if (CacheOpenAiAPIKey.IsEmpty())
		{
			UE_LOG(AI_AsyncRequestLog, Error, TEXT("Missing OpenAI API key in project settings. Please set it in Project Settings -> Plugins -> MinesweeperAI -> OpenAI API Key."));
			return;
		}
		FString ResponseString = Response->GetContentAsString();
	    TSharedPtr<FJsonObject> JsonResponse;
	    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);
	    if (FJsonSerializer::Deserialize(Reader, JsonResponse) && JsonResponse.IsValid())
	    {
	        const TArray<TSharedPtr<FJsonValue>>* ChoicesArray;
	        if (JsonResponse->TryGetArrayField(TEXT("choices"), ChoicesArray) && ChoicesArray->Num() > 0)
	        {
	            TSharedPtr<FJsonObject> FirstChoice = (*ChoicesArray)[0]->AsObject();
	            if (FirstChoice.IsValid() && FirstChoice->HasField(TEXT("message")))
	            {
	                TSharedPtr<FJsonObject> MessageObject = FirstChoice->GetObjectField(TEXT("message"));
	                if (MessageObject.IsValid() && MessageObject->HasField(TEXT("function_call")))
	                {
	                    TSharedPtr<FJsonObject> FunctionCall = MessageObject->GetObjectField(TEXT("function_call"));
	                    FString FunctionName = FunctionCall->GetStringField(TEXT("name"));
	                    if (FunctionName == "generate_minesweeper_grid")
	                    {
	                        FString ArgumentsString = FunctionCall->GetStringField(TEXT("arguments"));

	                        TSharedPtr<FJsonObject> ArgumentsJson;
	                        TSharedRef<TJsonReader<>> ArgsReader = TJsonReaderFactory<>::Create(ArgumentsString);
	                        if (FJsonSerializer::Deserialize(ArgsReader, ArgumentsJson) && ArgumentsJson.IsValid())
	                        {
	                            int32 Rows = ArgumentsJson->GetIntegerField(TEXT("rows"));
	                            int32 Cols = ArgumentsJson->GetIntegerField(TEXT("cols"));
	                            int32 Mines = ArgumentsJson->GetIntegerField(TEXT("mines"));

	                            TSharedPtr<FJsonObject> Grid = GenerateMinesweeperGrid(Rows, Cols, Mines);
	                            FString GridJson;
	                            TSharedRef<TJsonWriter<>> GridWriter = TJsonWriterFactory<>::Create(&GridJson);
	                            FJsonSerializer::Serialize(Grid.ToSharedRef(), GridWriter);
	                            //UE_LOG(AI_AsyncRequestLog, Log, TEXT("Generated Minesweeper Grid: %s"), *GridJson);
	                            TSharedPtr<FJsonObject> ResponsePayload = MakeShareable(new FJsonObject);
	                            ResponsePayload->SetStringField(TEXT("model"), JsonResponse->GetStringField(TEXT("model"))); // Attach model info
	                            // ✅ Include previous messages to maintain conversation history
	                            TArray<TSharedPtr<FJsonValue>> Messages;
	                            if (JsonResponse->HasField(TEXT("messages")))
	                            {
	                                Messages = JsonResponse->GetArrayField(TEXT("messages"));
	                            }

	                        	FString SystemMessage = TEXT(
									"You generate Minesweeper grids in JSON format.\n"
									"Always return ONLY a JSON object with a 'grid' key, nothing else.\n"
									"The grid size and number of mines are determined by the user's request.\n"
									"Mines are represented by 'X', and other tiles contain the correct count of adjacent mines (0-8).\n"
									"Do not add any explanations, comments, or extra text—ONLY return the JSON.\n"
									"\n"
									"### Correct Output Format ###\n"
									"{\n"
									"  \"grid\": [\n"
									"    [\"X\", 1, 0],\n"
									"    [1, 1, 0],\n"
									"    [0, 0, 0]\n"
									"  ]\n"
									"}"
								);

	                        	TSharedPtr<FJsonObject> SystemMessageObject = MakeShareable(new FJsonObject);
	                        	SystemMessageObject->SetStringField(TEXT("role"), TEXT("system"));
	                        	SystemMessageObject->SetStringField(TEXT("content"), SystemMessage);
	                        	Messages.Add(MakeShareable(new FJsonValueObject(SystemMessageObject)));

	                            TSharedPtr<FJsonObject> FunctionResponseMessage = MakeShareable(new FJsonObject);
	                            FunctionResponseMessage->SetStringField(TEXT("role"), TEXT("assistant"));

	                            TSharedPtr<FJsonObject> FunctionCallObject = MakeShareable(new FJsonObject);
	                            FunctionCallObject->SetStringField(TEXT("name"), TEXT("generate_minesweeper_grid"));
	                            FunctionCallObject->SetStringField(TEXT("arguments"), GridJson); // ✅ Convert object to string
	                            FunctionResponseMessage->SetObjectField(TEXT("function_call"), FunctionCallObject);
	                            Messages.Add(MakeShareable(new FJsonValueObject(FunctionResponseMessage)));
	                            ResponsePayload->SetArrayField(TEXT("messages"), Messages);

	                            FString ResponseContent;
	                            TSharedRef<TJsonWriter<>> ResponseWriter = TJsonWriterFactory<>::Create(&ResponseContent);
	                            FJsonSerializer::Serialize(ResponsePayload.ToSharedRef(), ResponseWriter);

	                            TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HTTPResponse = FHttpModule::Get().CreateRequest();
	                            HTTPResponse->SetURL(TEXT("https://api.openai.com/v1/chat/completions")); // OpenAI API URL
	                            HTTPResponse->SetVerb(TEXT("POST"));
	                            HTTPResponse->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	                            HTTPResponse->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *CacheOpenAiAPIKey));
	                            HTTPResponse->SetContentAsString(ResponseContent);

	                            HTTPResponse->OnProcessRequestComplete().BindUObject(this, &UOpenAIRequest::OnFinalResponse);
	                            HTTPResponse->ProcessRequest();
	                        }
	                        else
	                        {
	                            UE_LOG(AI_AsyncRequestLog, Error, TEXT("Failed to parse function arguments JSON."));
	                        }
	                    }
	                }
	            }
	        }
	    }
	}
	else
	{
		UE_LOG(AI_AsyncRequestLog, Error, TEXT("Failed to process request. Error: %s"), *Response->GetContentAsString());
	}
}

void UOpenAIRequest::OnFinalResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	FBaseResponseData ResponseData;
	ResponseData.Success = bWasSuccessful;
	ResponseData.Message = Response->GetContentAsString();
	ResponseData.Code = Response.IsValid() ? Response->GetResponseCode() : 0;
	FOpenAiResponseData OpenAiResponseData;
	if (bWasSuccessful && Response.IsValid())
	{
		FString ResponseString = Response->GetContentAsString();
		UE_LOG(AI_AsyncRequestLog, Log, TEXT("Final GPT response: %s"), *ResponseString);

		TSharedPtr<FJsonObject> JsonResponse;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);

		if (FJsonSerializer::Deserialize(Reader, JsonResponse) && JsonResponse.IsValid())
		{
			const TArray<TSharedPtr<FJsonValue>>* ChoicesArray;
			if (JsonResponse->TryGetArrayField(TEXT("choices"), ChoicesArray) && ChoicesArray->Num() > 0)
			{
				const TSharedPtr<FJsonObject> FirstChoice = (*ChoicesArray)[0]->AsObject();            
				if (FirstChoice.IsValid() && FirstChoice->HasField(TEXT("message")))
				{
					TSharedPtr<FJsonObject> MessageObject = FirstChoice->GetObjectField(TEXT("message"));
					if (MessageObject->HasField(TEXT("content")))
					{
						OpenAiResponseData.GridJson = MessageObject->GetStringField(TEXT("content"));
					}
				}
			}
		}
	}
	else
	{
		UE_LOG(AI_AsyncRequestLog, Error, TEXT("Failed to process request. Error: %s"), *Response->GetContentAsString());
	}
	OnCallback.Broadcast(OpenAiResponseData, ResponseData);
	UE_LOG(AI_AsyncRequestLog, Log, TEXT("GPT Final Message: %s"), *OpenAiResponseData.GridJson );
}

TSharedPtr<FJsonObject> UOpenAIRequest::GenerateMinesweeperGrid(int32 Rows, int32 Cols, int32 Mines)
{
	TArray<TArray<FString>> Grid;
	Grid.SetNum(Rows);

	for (int32 Row = 0; Row < Rows; Row++)
	{
		Grid[Row].SetNum(Cols);
		for (int32 Col = 0; Col < Cols; Col++)
		{
			Grid[Row][Col] = TEXT("0");
		}
	}

	// Randomly place mines
	TSet<FIntPoint> MinePositions;
	while (MinePositions.Num() < Mines)
	{
		int32 R = FMath::RandRange(0, Rows - 1);
		int32 C = FMath::RandRange(0, Cols - 1);
		MinePositions.Add(FIntPoint(R, C));
	}

	// Place mines and calculate adjacent numbers
	for (const FIntPoint& Mine : MinePositions)
	{
		Grid[Mine.X][Mine.Y] = "X";

		for (int32 dX = -1; dX <= 1; dX++)
		{
			for (int32 dY = -1; dY <= 1; dY++)
			{
				int32 NewRow = Mine.X + dX;
				int32 NewCol = Mine.Y + dY;

				if (NewRow >= 0 && NewRow < Rows && NewCol >= 0 && NewCol < Cols && Grid[NewRow][NewCol] != "X")
				{
					Grid[NewRow][NewCol] = FString::FromInt(FCString::Atoi(*Grid[NewRow][NewCol]) + 1);
				}
			}
		}
	}

	TSharedPtr<FJsonObject> JsonGrid = MakeShareable(new FJsonObject);
	TArray<TSharedPtr<FJsonValue>> GridArray;

	for (const TArray<FString>& Row : Grid)
	{
		TArray<TSharedPtr<FJsonValue>> JsonRow;
		for (const FString& Cell : Row)
		{
			JsonRow.Add(MakeShareable(new FJsonValueString(Cell)));
		}
		GridArray.Add(MakeShareable(new FJsonValueArray(JsonRow)));
	}

	JsonGrid->SetArrayField("grid", GridArray);
	return JsonGrid;
}

