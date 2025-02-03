// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AIAsyncRequest.generated.h"

USTRUCT(BlueprintType)
struct FBaseResponseData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category="SurrealGameliftSubsystem")
	bool Success = false;
	UPROPERTY(BlueprintReadWrite, Category="SurrealGameliftSubsystem")
	FString Message = FString("");
	UPROPERTY(BlueprintReadWrite, Category="SurrealGameliftSubsystem")
	int32 Code = 0;

	FBaseResponseData()
		:Success(false)
		,Message(FString(""))
		,Code(0)
	{}
};

USTRUCT(BlueprintType)
struct FOpenAiRequestData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category="SurrealGameliftSubsystem")
	FString Role = FString("");

	UPROPERTY(BlueprintReadWrite, Category="SurrealGameliftSubsystem")
	FString Prombt = FString("");

	FOpenAiRequestData()
		:Role(FString(""))
		,Prombt(FString(""))
	{}

};

USTRUCT(BlueprintType)
struct FOpenAiResponseData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category="SurrealGameliftSubsystem")
	FString GridJson = FString("");

	FOpenAiResponseData()
		:GridJson(FString(""))
	{}

};

UCLASS()
class UOpenAIRequest : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOpenAiResponse, const FOpenAiResponseData OpenAiResponseData, const FBaseResponseData ResponseData);
	FOpenAiResponse OnCallback;
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), DisplayName="OpenAIRequest")
	static UOpenAIRequest* OpenAIRequest(FOpenAiRequestData Request);
protected:
	TSharedPtr<FJsonObject> GenerateMinesweeperGrid(int32 Rows, int32 Cols, int32 Mines);
	void OnOpenAIRequest(FOpenAiRequestData Request);
	void OnHttpResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void OnFinalResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
private:
	FString CacheOpenAiAPIKey = FString("");
};

