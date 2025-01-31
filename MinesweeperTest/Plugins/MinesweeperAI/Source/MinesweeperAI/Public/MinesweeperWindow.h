// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"

/**
 * 
 */
class MINESWEEPERAI_API SMinesweeperWindow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMinesweeperWindow)
		{
		}

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

private:
	TSharedPtr<SMultiLineEditableTextBox> InputTextBox;
	// 🔹 Creates a Minesweeper Tile Button
	TSharedRef<SWidget> CreateTileButton()
	{
		return SNew(SButton)
		.ButtonColorAndOpacity(FLinearColor(0.3f, 0.3f, 0.3f, 1.0f)) // ✅ Default grey background
		.OnHovered_Lambda([]() { UE_LOG(LogTemp, Log, TEXT("Button Hovered")); }) // Optional hover log
		.OnClicked(this, &SMinesweeperWindow::OnTileClicked)
		[
			SNew(STextBlock)
			.Text(FText::FromString(""))
			.Justification(ETextJustify::Center)
		];
	}

	// 🔹 Called when a Minesweeper Tile is clicked
	FReply OnTileClicked()
	{
		UE_LOG(LogTemp, Warning, TEXT("Tile clicked!"));
		return FReply::Handled();
	}

	// 🔹 Called when the Generate New Grid button is clicked
	FReply OnGenerateGridClicked()
	{
		UE_LOG(LogTemp, Warning, TEXT("Generating new grid..."));
		return FReply::Handled();
	}

	void OnTextCommitted(const FText& NewText, ETextCommit::Type CommitType)
	{
		if (CommitType == ETextCommit::OnEnter)
		{
			UE_LOG(LogTemp, Log, TEXT("User entered: %s"), *NewText.ToString());
		}
	}

	FReply OnClearInputClicked()
	{
		if (InputTextBox.IsValid())
		{
			InputTextBox->SetText(FText::GetEmpty()); // ✅ Clears the text box
		}
		return FReply::Handled();
	}

	// 🔹 Submits the message
	FReply OnSubmitClicked()
	{
		if (InputTextBox.IsValid())
		{
			FString UserInput = InputTextBox->GetText().ToString();
			UE_LOG(LogTemp, Log, TEXT("User submitted: %s"), *UserInput);
		}
		return FReply::Handled();
	}
};
