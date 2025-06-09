// Copyright 2025 Teyon. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RaceWidget.generated.h"

class UTextBlock;
class UHorizontalBox;
class ARacingCar;
class UImage;

/**
 * 
 */
UCLASS()
class PRAKTYKI_API URaceWidget : public UUserWidget
{
	GENERATED_BODY()

private:

	FTimerHandle SectorColourTimer;

	FTimerHandle SectorDifferenceTimer;
	
public:

	virtual void NativeConstruct() override;

	UFUNCTION()
	void CreateSectorBox();

	UFUNCTION()
	void UpdateSectorBox();

	UFUNCTION()
	void ShowPenalty();

	UFUNCTION()
	void HidePenalty();

	UFUNCTION()
	void ResetSectorBoxes();

	UFUNCTION()
	void ShowSectorDifference();

	UFUNCTION()
	void HideSectorDifference();

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Timer;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Laps;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* BestLap;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PreviousLap;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SectorTimer;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentLap;

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* SectorBox;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PenaltyValue;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PenaltyValueText;

	UPROPERTY(meta = (BindWidget))
	UImage* PenaltyBackground;

	UPROPERTY(meta = (BindWidget))
	UImage* BackgroundData;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PenaltyText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SectorTimeDifference;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Speed;

	UPROPERTY(BlueprintReadWrite)
	ARacingCar* OwningRacingCar;

	UPROPERTY()
	TArray<UImage*> SectorImages;

};
