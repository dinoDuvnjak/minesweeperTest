// Fill out your copyright notice in the Description page of Project Settings.


#include "MinesweeperWindow.h"

#include "MinesweeperWindowStyle.h"
#include "SlateOptMacros.h"
#include "Components/SizeBox.h"
#include "Components/SizeBoxSlot.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

/*void SMinesweeperWindow::Construct(const FArguments& InArgs)
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

Font(FCoreStyle::GetDefaultFontStyle("Regular", 13))
    ];
}*/

void SMinesweeperWindow::Construct(const FArguments& InArgs)
{
    ChildSlot
    [
        SNew(SVerticalBox)

        + SVerticalBox::Slot()
         .AutoHeight()
        .Padding(10)
       [
           SNew(SBorder)
            //.HAlign(HAlign_Right)
            .BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
            .BorderBackgroundColor(FLinearColor(0.f,0.f,0.f,0.7f)) // ✅ Red background
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
                        SNew(SButton)
                        .HAlign(HAlign_Right)
                        .VAlign(VAlign_Center)
                        //.ContentPadding(10)
                        //.ButtonColorAndOpacity(FLinearColor::Black)
                        .ButtonStyle( FAppStyle::Get(), "SecondaryButton" )
                        .ContentPadding(0)
                        .OnClicked(this, &SMinesweeperWindow::OnClearInputClicked)
                        /*[
                            SNew(STextBlock)
                            .Justification(ETextJustify::Center)
                        ]*/
                        .Content()
                        [
                            SNew( SImage )
                            .Image(FSlateIcon(FName("DefaultRevisionControlStyle"), "RevisionControl.Actions.Refresh").GetIcon())
                            /*SNew(SBox)
                            .HAlign(HAlign_Right)
                            .VAlign(VAlign_Center)
                            [
                            ]*/
                        ]
                    ]
                    /*SNew(SOverlay)
                    + SOverlay::Slot()
                    [
                       SNew(SBorder)
                       .BorderImage(FAppStyle::GetBrush("ColorPicker.RoundedInputBorder"))
                    ]

                    + SOverlay::Slot()
                    .HAlign(HAlign_Right)
                    [
                    ]*/
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
                    // Multi-line Text Input
                    + SOverlay::Slot()
                    .VAlign(VAlign_Center)
                    .Padding(0)
                    [
                        SNew(SEditableTextBox)
                        .Text(FText::FromString("Generate a new 3x3 Minesweeper grid with 2 mines"))
                        .Padding(20)
                        .OnTextCommitted(this, &SMinesweeperWindow::OnTextCommitted)
                        //.ToolTipText(FText::FromName(Options->Test))
                    ]

                    // Submit Button (Overlayed on the Right)
                    + SOverlay::Slot()
                    .HAlign(HAlign_Right)
                    .VAlign(VAlign_Center)
                    .Padding(FMargin(0, 0, 20, 0))
                    [
                        SNew(SBox)
                        .WidthOverride(32)
                        .HeightOverride(32)
                        [
                            SNew(SButton)
                            .HAlign(HAlign_Center) // ✅ Center horizontally
                            .VAlign(VAlign_Center)
                            .ButtonStyle( FAppStyle::Get(), "SecondaryButton" )
                            .ContentPadding(0)
                            //.ButtonColorAndOpacity(FLinearColor(0.5f, 0.5f, 0.5f, 0.5f))
                            .OnClicked(this, &SMinesweeperWindow::OnSubmitClicked)
                            /*[
                                SNew(STextBlock)
                                .Justification(ETextJustify::Center)
                            ]*/
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

            /*// 🔹 2. AI STATUS ROW (Icon + "Waiting for instruction" Text)
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(0)
            [
                SNew(SBorder)
                .Padding(FMargin(10))
                [
                    SNew(SHorizontalBox)
                    + SHorizontalBox::Slot()
                    .FillWidth(1)
                    .AutoWidth()
                    .Padding(5)
                    [
                        SNew(SImage)
                        .Image(FCoreStyle::Get().GetBrush("Icons.Help"))
                    ]

                    // AI Text
                    + SHorizontalBox::Slot()
                    .VAlign(VAlign_Center)
                    .Padding(5)
                    [
                        SNew(STextBlock)
                        .Text(FText::FromString("waiting for your instruction.."))
                        .Font(FCoreStyle::GetDefaultFontStyle("Regular", 13))
                    ]
                ]
            ]

            // 🔹 3. INPUT ROW (Multi-line Text Field + Submit Button Overlayed)
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(0)
            [
                SNew(SOverlay) // ✅ Overlay to position submit button over input field

                // Multi-line Text Input
                + SOverlay::Slot()
                [
                    SAssignNew(InputTextBox, SMultiLineEditableTextBox)
                    .WrapTextAt(500) // ✅ Auto-wrap long text
                    .Text(FText::FromString("Enter your command here..."))
                    .OnTextCommitted(this, &SMinesweeperWindow::OnTextCommitted)
                    .Padding(FMargin(5, 5, 40, 5)) // Leave space for the submit button
                ]

                // Submit Button (Overlayed on the Right)
                + SOverlay::Slot()
                .HAlign(HAlign_Right)
                .VAlign(VAlign_Center)
                .Padding(FMargin(0, 0, 5, 0)) // Align to the right
                [
                    SNew(SButton)
                    .ButtonColorAndOpacity(FLinearColor(0.2f, 0.6f, 1.0f, 1.0f)) // ✅ Blue button
                    .OnClicked(this, &SMinesweeperWindow::OnSubmitClicked)
                    [
                        SNew(STextBlock)
                        .Text(FText::FromString("➤")) // Unicode arrow symbol
                        .Justification(ETextJustify::Center)
                    ]
                ]
            ]*/
       ]

    ];
}



END_SLATE_FUNCTION_BUILD_OPTIMIZATION
