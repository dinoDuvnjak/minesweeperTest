#pragma once
#include "SlateStyleData.h"

class FSlateStyleWidgetSplitter : public FSlateStyleData
{
public:
	virtual TSharedRef<SWidget> GenerateRowWidget() override
	{
		const FSplitterStyle* s = GetWidgetStyle<FSplitterStyle>();
		if (!s)
			return SNullWidget::NullWidget;

		return SNew(SSplitter)
			.Style(s)
			.Orientation(Orient_Horizontal)
			+SSplitter::Slot()
#if ENGINE_MAJOR_VERSION == 5
			.Resizable(true)
#endif
			.Value(0.5)
			[
				SNew(SBorder)
				[
					SNew(STextBlock)
					.Text(DefaultValues.Left)
				]
			]
			+SSplitter::Slot()
#if ENGINE_MAJOR_VERSION == 5
			.Resizable(true)
#endif
			[
				SNew(SBorder)
				[
					SNew(STextBlock)
					.Text(DefaultValues.Right)
				]
			];
	};

	virtual void InitializeDetails() override
	{
		FillDetailsWithProperties<FSplitterStyle>();
	};
};
