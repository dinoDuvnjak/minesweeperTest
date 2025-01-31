#include "MinesweeperWindowStyle.h"

#include "Styling/SlateStyleRegistry.h"

FName FMinesweeperWindowStyle::StyleName("FMinesweeperWindowStyle");
TUniquePtr<FMinesweeperWindowStyle> FMinesweeperWindowStyle::Inst(nullptr);

const FName& FMinesweeperWindowStyle::GetStyleSetName() const
{
	return StyleName;
}

const FMinesweeperWindowStyle& FMinesweeperWindowStyle::Get()
{
	if (!Inst.IsValid())
	{
		Inst = TUniquePtr<FMinesweeperWindowStyle>(new FMinesweeperWindowStyle);
	}
	return *(Inst.Get());
}

void FMinesweeperWindowStyle::Shutdown()
{
	Inst.Reset();
}

FMinesweeperWindowStyle::FMinesweeperWindowStyle()
	: FSlateStyleSet(StyleName)
{
	FSlateStyleRegistry::RegisterSlateStyle(*this);
}

FMinesweeperWindowStyle::~FMinesweeperWindowStyle()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*this);
}
