// Fill out your copyright notice in the Description page of Project Settings.


#include "MinesweeperWindow.h"

#include "SlateOptMacros.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SMinesweeperWindow::Construct(const FArguments& InArgs)
{
	ChildSlot
    [
        SNew(SVerticalBox)

        // 🔹 Minesweeper Grid (4x4)
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(10)
        [
            SNew(SGridPanel)
            + SGridPanel::Slot(0, 0)[CreateTileButton()]
            + SGridPanel::Slot(1, 0)[CreateTileButton()]
            + SGridPanel::Slot(2, 0)[CreateTileButton()]
            + SGridPanel::Slot(3, 0)[CreateTileButton()]

            + SGridPanel::Slot(0, 1)[CreateTileButton()]
            + SGridPanel::Slot(1, 1)[CreateTileButton()]
            + SGridPanel::Slot(2, 1)[CreateTileButton()]
            + SGridPanel::Slot(3, 1)[CreateTileButton()]

            + SGridPanel::Slot(0, 2)[CreateTileButton()]
            + SGridPanel::Slot(1, 2)[CreateTileButton()]
            + SGridPanel::Slot(2, 2)[CreateTileButton()]
            + SGridPanel::Slot(3, 2)[CreateTileButton()]

            + SGridPanel::Slot(0, 3)[CreateTileButton()]
            + SGridPanel::Slot(1, 3)[CreateTileButton()]
            + SGridPanel::Slot(2, 3)[CreateTileButton()]
            + SGridPanel::Slot(3, 3)[CreateTileButton()]
        ]

        // 🔹 Header Box (Contains Input Field + Refresh Button)
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(10)
        [
            SNew(SHorizontalBox)

            // Multi-line Input Field
            + SHorizontalBox::Slot()
            .FillWidth(1)
            .Padding(5)
            [
                SAssignNew(InputTextBox, SMultiLineEditableTextBox) // ✅ Assign for clearing later
                .WrapTextAt(500) // ✅ Auto-wrap long text
                .Text(FText::FromString("Enter your command here..."))
                .OnTextCommitted(this, &SMinesweeperWindow::OnTextCommitted)
            ]

            // Refresh Button
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(5)
            [
                SNew(SButton)
                .ButtonColorAndOpacity(FLinearColor(0.8f, 0.1f, 0.1f, 1.0f)) // Red refresh button
                .OnClicked(this, &SMinesweeperWindow::OnClearInputClicked)
                [
                    SNew(STextBlock)
                    .Text(FText::FromString("🔄")) // Unicode refresh symbol or use an icon
                    .Justification(ETextJustify::Center)
                ]
            ]
        ]

        // 🔹 AI Instruction Panel
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(10)
        [
            SNew(SBorder)
            .Padding(10)
            [
                SNew(SHorizontalBox)

                // AI Icon
                + SHorizontalBox::Slot()
                .AutoWidth()
                .Padding(5)
                [
                    SNew(SImage)
                    .Image(FCoreStyle::Get().GetBrush("Icons.Help"))
                ]

                // AI Text
                + SHorizontalBox::Slot()
                .FillWidth(1)
                .VAlign(VAlign_Center)
                .Padding(5)
                [
                    SNew(STextBlock)
                    .Text(FText::FromString("waiting for your instruction.."))
                    .Font(FCoreStyle::GetDefaultFontStyle("Regular", 13))
                ]
            ]
        ]
    ];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
