// Copyright 2025 Teyon. All Rights Reserved.


#include "StartRaceWidget.h"
#include "Components/Image.h"


UImage* UStartRaceWidget::GetLightByIndex(int32 Index)
{
	switch (Index)
	{
	case 0: return Light1;
	case 1: return Light2;
	case 2: return Light3;
	case 3: return Light4;
	case 4: return Light5;
	default: return nullptr;
	}
}

void UStartRaceWidget::SetLightColour(int32 Index, FLinearColor Colour)
{
	UImage* Light = GetLightByIndex(Index);
	Light->SetColorAndOpacity(Colour);
}
