// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIAsyncRequest.h"
#include "Widgets/SCompoundWidget.h"

DEFINE_LOG_CATEGORY_STATIC(MinesweeperWindowLog, Log, All);

#define PLACEHOLDER_TEXT_PRE_GRID_GENERATOR TEXT("Generate a new 3x3 Minesweeper grid with 2 mines")
#define PLACEHOLDER_TEXT_ON_GRID_GENERATING TEXT("Generating grid...")


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

	void Construct(const FArguments& InArgs);

private:
	TSharedPtr<SGridPanel> MinesweeperGrid = nullptr;
	TArray<TArray<TSharedPtr<SButton>>> GridButtons = TArray<TArray<TSharedPtr<SButton>>>();
	TArray<TArray<FString>> GridData = TArray<TArray<FString>>();
	TSharedPtr<SEditableTextBox> InputTextBox = nullptr;
	FString CachedPrombt = FString("");
	TSharedPtr<SButton> SubmitButton;
	TSharedPtr<SButton> RefreshButton;
	bool bIsGeneratingGrid = false;
	
private:	
	FReply OnTileClicked(int32 Row, int32 Col)
	{
		if (GridData.IsValidIndex(Row) && GridData[Row].IsValidIndex(Col))
		{
			FString TileValue = GridData[Row][Col];

			if (TileValue == "X")
			{
				FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("Game Over! You hit a bomb!"));
				AsyncTask(ENamedThreads::GameThread, [this]()
				{
					RegenerateBoard();
				});

				return FReply::Handled();
			}
			else
			{
				RevealTile(Row, Col);
			}
		}

		return FReply::Handled();
	}

	void RegenerateBoard()
	{
		if (CachedPrombt.IsEmpty())
		{
			UE_LOG(MinesweeperWindowLog, Error, TEXT("RegenerateBoard, CachedPrombt is empty"));
			return;
		}
		SendRequest(CachedPrombt);
	}

	void RevealTile(int32 Row, int32 Col)
	{
		if (!GridData.IsValidIndex(Row) || !GridData[Row].IsValidIndex(Col) || 
			!GridButtons.IsValidIndex(Row) || !GridButtons[Row].IsValidIndex(Col))
		{
			return;
		}

		if (GridData[Row][Col] == "R") 
		{
			return;
		}

		FString TileValue = GridData[Row][Col];
		GridData[Row][Col] = "R";

		if (GridButtons[Row][Col].IsValid()) 
		{
			GridButtons[Row][Col]->SetContent(
				SNew(STextBlock).Text(FText::FromString(TileValue == "0" ? "" : TileValue))
			);
			if (TileValue == "0")
			{
				GridButtons[Row][Col]->SetVisibility(EVisibility::Hidden);
			}
		}

		if (TileValue == "0")
		{
			RevealTile(Row - 1, Col); 
			RevealTile(Row + 1, Col); 
			RevealTile(Row, Col - 1); 
			RevealTile(Row, Col + 1);
		}
	}

	void OnTextCommitted(const FText& NewText, ETextCommit::Type CommitType)
	{
		if (CommitType == ETextCommit::OnEnter)
		{
			UE_LOG(MinesweeperWindowLog, Log, TEXT("User entered: %s"), *NewText.ToString());
		}
	}

	FReply OnRefreshButtonClicked()
	{
		RegenerateBoard();
		return FReply::Handled();
	}

	FReply OnSubmitClicked()
	{
		if (bIsGeneratingGrid)
		{
			return FReply::Handled();
		}
		if (InputTextBox.IsValid())
		{
			FString UserInput = InputTextBox->GetText().ToString();
			SendRequest(UserInput);
		}
		if (RefreshButton.IsValid())
		{
			RefreshButton->SetEnabled(true);
		}
		return FReply::Handled();
	}

	void SendRequest(const FString& Prombt)
	{
		FOpenAiRequestData RequestData;
		RequestData.Prombt = Prombt;
		CachedPrombt = Prombt;
		UOpenAIRequest* OpenAIRequest = UOpenAIRequest::OpenAIRequest(RequestData);
		if (OpenAIRequest)
		{
			OnGridGenerationStart();
			OpenAIRequest->OnCallback.AddLambda([this](FOpenAiResponseData ResponseData, FBaseResponseData BaseResponseData)
			{
				ParseMinesweeperGrid(ResponseData.GridJson);
			});
		}
	}
	
	void OnGridGenerationStart()
	{
		SetChatInputFieldEnabled(false);
		InputTextBox->SetText(FText::FromString(PLACEHOLDER_TEXT_ON_GRID_GENERATING));
		bIsGeneratingGrid = true;
	}

	void OnGridGenerationFinished()
	{
		SetChatInputFieldEnabled(true);
		InputTextBox->SetText(FText::FromString(PLACEHOLDER_TEXT_PRE_GRID_GENERATOR));
		bIsGeneratingGrid = false;
	}

	void SetChatInputFieldEnabled(const bool bEnabled)
	{
		if (InputTextBox.IsValid())
		{
			InputTextBox->SetIsReadOnly(!bEnabled);
		}
		if (SubmitButton.IsValid())
		{
			SubmitButton->SetEnabled(bEnabled);
		}
	}
	
	void ParseMinesweeperGrid(const FString& GridJson)
	{		
		TSharedPtr<FJsonObject> JsonParsed;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(GridJson);

		if (FJsonSerializer::Deserialize(Reader, JsonParsed) && JsonParsed.IsValid())
		{
			const TArray<TSharedPtr<FJsonValue>>* GridArray;
			if (JsonParsed->TryGetArrayField(TEXT("grid"), GridArray))
			{
				GridData.Empty();
				for (const auto& RowValue : *GridArray)
				{
					TArray<FString> Row;
					const TArray<TSharedPtr<FJsonValue>>* RowArray;

					if (RowValue->TryGetArray(RowArray))
					{
						for (const auto& CellValue : *RowArray)
						{
							Row.Add(CellValue->AsString());
						}
					}
					GridData.Add(Row);
				}
				RebuildGrid();
			}
		}
	}

	void RebuildGrid()
	{
		if (!MinesweeperGrid.IsValid())
		{
			return;
		}

		MinesweeperGrid->ClearChildren();
		GridButtons.Empty();

		int32 Rows = GridData.Num();
		int32 Cols = (Rows > 0) ? GridData[0].Num() : 0;

		GridButtons.SetNum(Rows);

		for (int32 Row = 0; Row < Rows; Row++)
		{
			GridButtons[Row].SetNum(Cols);
			for (int32 Col = 0; Col < Cols; Col++)
			{
				TSharedPtr<SButton> TileButton;

				MinesweeperGrid->AddSlot(Col, Row)
				[
					SAssignNew(TileButton, SButton)
					.OnClicked(this, &SMinesweeperWindow::OnTileClicked, Row, Col)
					.Content()
					[
						SNew(STextBlock).Text(FText::FromString(" ")) // Initially empty
					]
				];

				GridButtons[Row][Col] = TileButton; // Store button reference
			}
		}
		OnGridGenerationFinished();
	}
};
