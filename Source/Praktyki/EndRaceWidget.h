// Copyright 2025 Teyon. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EndRaceWidget.generated.h"

class UTextBlock;
class UButton;
class ARacingCar;

/**
 * 
 */
UCLASS()
class PRAKTYKI_API UEndRaceWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* BestTime;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* RaceTime;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* FastestLap;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Penalties;

	UPROPERTY(meta = (BindWidget))
	UButton* MenuButton;

	UPROPERTY(meta = (BindWidget))
	UButton* RaceAgainButton;

	UFUNCTION()
	void OnRaceAgainClicked();

	UFUNCTION()
	void OnMenuClicked();

	UPROPERTY(BlueprintReadWrite)
	ARacingCar* OwningRacingCar;
	
};
