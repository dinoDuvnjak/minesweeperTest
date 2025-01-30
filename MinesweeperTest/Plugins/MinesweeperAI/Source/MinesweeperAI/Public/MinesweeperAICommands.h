// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "MinesweeperAIStyle.h"

class FMinesweeperAICommands : public TCommands<FMinesweeperAICommands>
{
public:

	FMinesweeperAICommands()
		: TCommands<FMinesweeperAICommands>(TEXT("MinesweeperAI"), NSLOCTEXT("Contexts", "MinesweeperAI", "MinesweeperAI Plugin"), NAME_None, FMinesweeperAIStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};