#pragma once

#include "Styling/SlateStyle.h"
#include "Styling/SlateColor.h"
#include "UObject/NameTypes.h"
#include "Templates/UniquePtr.h"

class FMinesweeperWindowStyle
	: public FSlateStyleSet
{
public:
	virtual const FName& GetStyleSetName() const override;

	static const FMinesweeperWindowStyle& Get();
	static void Shutdown();

	~FMinesweeperWindowStyle();

private:
	FMinesweeperWindowStyle();

	static FName StyleName;
	static TUniquePtr<FMinesweeperWindowStyle> Inst;
};