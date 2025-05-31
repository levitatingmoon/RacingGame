// Copyright 2025 Teyon. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "EndRaceWidget.generated.h"

/**
 * 
 */
UCLASS()
class PRAKTYKI_API UEndRaceWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	UTextBlock* BestTime;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* RaceTime;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* FastestLap;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Penalties;
	
};
