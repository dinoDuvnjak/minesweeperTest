#pragma once

class FSlateStyleData;

class SSlateStyleTableRow : public SMultiColumnTableRow<TSharedPtr<FSlateStyleData>>
{
public:
	SLATE_BEGIN_ARGS(SSlateStyleTableRow)
		{}
		SLATE_ARGUMENT(TSharedPtr<FSlateStyleData>, StyleData);
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView);

	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override;
	~SSlateStyleTableRow() = default;

private:
	TWeakPtr<FSlateStyleData> StyleData;
};

class SSlateStyleToolTip : public SToolTip
{
public:
	SLATE_BEGIN_ARGS(SSlateStyleToolTip)
		{}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TWeakPtr<FSlateStyleData>& InStyleData);

	virtual void OnOpening() override;

private:
	TWeakPtr<FSlateStyleData> StyleData;
};
