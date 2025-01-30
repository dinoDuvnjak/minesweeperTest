// Copyright Epic Games, Inc. All Rights Reserved.

#include "MinesweeperAICommands.h"

#define LOCTEXT_NAMESPACE "FMinesweeperAIModule"

void FMinesweeperAICommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "MinesweeperAI", "Bring up MinesweeperAI window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
