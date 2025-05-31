// Copyright 2025 Teyon. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EndRaceWidget.generated.h"

class UTextBlock;
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
