// Copyright 2025 Teyon. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StartRaceWidget.generated.h"

class UImage;
/**
 * 
 */
UCLASS()
class PRAKTYKI_API UStartRaceWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UImage* Light1;

	UPROPERTY(meta = (BindWidget))
	UImage* Light2;

	UPROPERTY(meta = (BindWidget))
	UImage* Light3;

	UPROPERTY(meta = (BindWidget))
	UImage* Light4;

	UPROPERTY(meta = (BindWidget))
	UImage* Light5;

	UImage* GetLightByIndex(int32 Index);

	void SetLightColour(int32 Index, FLinearColor Colour);
	
};
