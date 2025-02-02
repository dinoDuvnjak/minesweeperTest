// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MinesweeperDeveloperSettings.generated.h"

/**
 * 
 */
UCLASS(config = Game, DefaultConfig)
class MINESWEEPERAI_API UMinesweeperDeveloperSettings : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "MinesweeperDeveloperSettings")
	FString OpenAi_API_Key = FString("");
};
