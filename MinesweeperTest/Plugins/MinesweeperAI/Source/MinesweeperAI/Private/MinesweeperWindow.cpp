// Fill out your copyright notice in the Description page of Project Settings.


#include "MinesweeperWindow.h"
#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SMinesweeperWindow::Construct(const FArguments& InArgs)
{
    ChildSlot
    [
        SNew(SVerticalBox)

        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(10)
       [
           SAssignNew(MinesweeperGrid, SGridPanel)
       ]

        + SVerticalBox::Slot()
         .AutoHeight()
        .Padding(10)
       [
           SNew(SBorder)
            //.HAlign(HAlign_Right)
            .BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
            .BorderBackgroundColor(FLinearColor(0.f,0.f,0.f,0.7f))
            .Padding(20)
            [
               SNew(SVerticalBox)
                // HEADER ROW 
                + SVerticalBox::Slot()
                [
                    SNew(SBox)
                    .HAlign(HAlign_Right)
                    .WidthOverride(40)
                    .HeightOverride(40)
                    [
                        SAssignNew(RefreshButton, SButton)
                        .HAlign(HAlign_Right)
                        .VAlign(VAlign_Center)
                        .IsEnabled(false)
                        .ToolTipText(FText::FromString("Refresh the grid"))
                        .ButtonStyle( FAppStyle::Get(), "SecondaryButton" )
                        .ContentPadding(0)
                        .OnClicked(this, &SMinesweeperWindow::OnRefreshButtonClicked)
                        .Content()
                        [
                            SNew( SImage )
                            .Image(FSlateIcon(FName("DefaultRevisionControlStyle"), "RevisionControl.Actions.Refresh").GetIcon())
                        ]
                    ]
                ]
                // AI ROW
                + SVerticalBox::Slot()
                   .AutoHeight()
                   .Padding(FMargin(0.f, 10.f, 0.f, 0.f))
                   [
                       SNew(SBorder)
                       .BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
                       .BorderBackgroundColor(FLinearColor(0.5f,0.5f,0.5f,0.5f))
                       .Padding(FMargin(5))
                       [
                           SNew(SHorizontalBox)
                           // AI image
                           + SHorizontalBox::Slot()
                           .FillWidth(1)
                           .AutoWidth()
                           .Padding(10)
                           [
                               SNew(SImage)
                               .Image(FSlateIcon(FName("PaperStyle"), "ClassThumbnail.PaperCharacter").GetIcon())
                           ]
                           // AI Text
                           + SHorizontalBox::Slot()
                           .VAlign(VAlign_Center)
                           .Padding(5)
                           [
                               SNew(SVerticalBox)
                               //AI TITLE
                               + SVerticalBox::Slot()
                               .VAlign(VAlign_Top)
                                [
                                   SNew(STextBlock)
                                   .Text(FText::FromString("AI"))
                                   .Font(FCoreStyle::GetDefaultFontStyle("Bold", 20))
                                ]
                                + SVerticalBox::Slot()
                               .VAlign(VAlign_Bottom)
                                [
                                   SNew(STextBlock)
                                   .Text(FText::FromString("waiting for your instruction.."))
                                   .Font(FCoreStyle::GetDefaultFontStyle("Regular", 13))
                                ]
                           ]
                       ]
                   ]
                //INPUT ROW
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(FMargin(0.f, 10.f, 0.f, 0.f))
                [
                    SNew(SOverlay)
                    + SOverlay::Slot()
                    .VAlign(VAlign_Center)
                    .Padding(0)
                    [
                        SAssignNew(InputTextBox, SEditableTextBox)
                        .Text(FText::FromString("Generate a new 3x3 Minesweeper grid with 2 mines"))
                        .Padding(20)
                        .OnTextCommitted(this, &SMinesweeperWindow::OnTextCommitted)
                    ]

                    + SOverlay::Slot()
                    .HAlign(HAlign_Right)
                    .VAlign(VAlign_Center)
                    .Padding(FMargin(0, 0, 20, 0))
                    [
                        SNew(SBox)
                        .WidthOverride(32)
                        .HeightOverride(32)
                        [
                            SAssignNew(SubmitButton, SButton)
                            .HAlign(HAlign_Center)
                            .VAlign(VAlign_Center)
                            .ButtonStyle( FAppStyle::Get(), "SecondaryButton" )
                            .ContentPadding(0)
                            .OnClicked(this, &SMinesweeperWindow::OnSubmitClicked)
                            .Content()
                           [
                               SNew(SBox)
                               .HAlign(HAlign_Center)
                               .VAlign(VAlign_Center)
                               [
                                SNew(SImage)
                                .Image(FSlateIcon(FName("CoreStyle"), "Icons.ArrowUp").GetIcon())                            
                               ]
                           ]
                        ]
                    ]
                ]
            ]
       ]
    ];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
