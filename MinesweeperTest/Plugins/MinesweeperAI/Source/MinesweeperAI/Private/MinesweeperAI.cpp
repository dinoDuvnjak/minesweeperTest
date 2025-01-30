// Copyright Epic Games, Inc. All Rights Reserved.

#include "MinesweeperAI.h"
#include "MinesweeperAIStyle.h"
#include "MinesweeperAICommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"

static const FName MinesweeperAITabName("MinesweeperAI");

#define LOCTEXT_NAMESPACE "FMinesweeperAIModule"

void FMinesweeperAIModule::StartupModule()
{
    FMinesweeperAIStyle::Initialize();
    FMinesweeperAIStyle::ReloadTextures();

    FMinesweeperAICommands::Register();

    PluginCommands = MakeShareable(new FUICommandList);
    PluginCommands->MapAction(
        FMinesweeperAICommands::Get().OpenPluginWindow,
        FExecuteAction::CreateRaw(this, &FMinesweeperAIModule::PluginButtonClicked),
        FCanExecuteAction()
    );

    UToolMenus::RegisterStartupCallback(
        FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FMinesweeperAIModule::RegisterMenus)
    );

    FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
        MinesweeperAITabName,
        FOnSpawnTab::CreateRaw(this, &FMinesweeperAIModule::OnSpawnPluginTab)
    )
    .SetDisplayName(LOCTEXT("FMinesweeperAITabTitle", "MinesweeperAI"))
    .SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FMinesweeperAIModule::ShutdownModule()
{
    UToolMenus::UnRegisterStartupCallback(this);
    UToolMenus::UnregisterOwner(this);

    FMinesweeperAIStyle::Shutdown();
    FMinesweeperAICommands::Unregister();

    FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(MinesweeperAITabName);
}

TSharedRef<SDockTab> FMinesweeperAIModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
    FText WidgetText = FText::Format(
        LOCTEXT("WindowWidgetText", "Add code to {0} in {1} to override this window's contents"),
        FText::FromString(TEXT("FMinesweeperAIModule::OnSpawnPluginTab")),
        FText::FromString(TEXT("MinesweeperAI.cpp"))
    );

    return SNew(SDockTab)
        .TabRole(ETabRole::NomadTab)
        [
            SNew(SBox)
            .HAlign(HAlign_Center)
            .VAlign(VAlign_Center)
            [
                SNew(STextBlock)
                .Text(WidgetText)
            ]
        ];
}

void FMinesweeperAIModule::PluginButtonClicked()
{
    FGlobalTabmanager::Get()->TryInvokeTab(MinesweeperAITabName);
}

void FMinesweeperAIModule::RegisterMenus()
{
    FToolMenuOwnerScoped OwnerScoped(this);

    {
        UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
        {
            FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
            Section.AddMenuEntryWithCommandList(
                FMinesweeperAICommands::Get().OpenPluginWindow,
                PluginCommands
            );
        }
    }

    {
        UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
        {
            FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
            {
                FToolMenuEntry& Entry = Section.AddEntry(
                    FToolMenuEntry::InitToolBarButton(
                        FMinesweeperAICommands::Get().OpenPluginWindow
                    )
                );
                Entry.SetCommandList(PluginCommands);
            }
        }
    }

    UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu(
        "LevelEditor.LevelEditorToolBar.ModesToolBar"
    );
    FToolMenuSection& ToolbarSection = ToolbarMenu->FindOrAddSection("File");

    ToolbarSection.AddEntry(FToolMenuEntry::InitToolBarButton(
        TEXT("MinesweeperAI"),
        FExecuteAction::CreateRaw(this, &FMinesweeperAIModule::PluginButtonClicked), 
        INVTEXT("MinesweeperAI button"),
        INVTEXT("Open MinesweeperAI standalone window"),
        FSlateIcon(FName("InsightsStyle"), "Icons.Launcher")
    ));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMinesweeperAIModule, MinesweeperAI)
