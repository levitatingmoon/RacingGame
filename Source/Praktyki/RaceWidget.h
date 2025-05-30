// Copyright 2025 Teyon. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "RaceWidget.generated.h"

/**
 * 
 */
UCLASS()
class PRAKTYKI_API URaceWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Timer;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Laps;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* BestLap;
};
