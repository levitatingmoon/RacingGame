// Copyright 2025 Teyon. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class UTextBlock;
class USlider;
class UButton;

/**
 * 
 */
UCLASS()
class PRAKTYKI_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TimeLimitSliderValue;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* LapLimitSliderValue;

	UPROPERTY(meta = (BindWidget))
	USlider* TimeLimitSlider;

	UPROPERTY(meta = (BindWidget))
	USlider* LapLimitSlider;

	UPROPERTY(meta = (BindWidget))
	UButton* StartGameButton;

	UPROPERTY(meta = (BindWidget))
	UButton* QuitGameButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ResetSettingsButton;

	float TimeLimitSliderDefault = 300.0f;

	int LapLimitSliderDefault = 10;

	UFUNCTION()
	void OnLapLimitSliderChanged(float Value);

	UFUNCTION()
	void OnTimeLimitSliderChanged(float Value);
	
	UFUNCTION()
	void OnQuitClicked();

	UFUNCTION()
	void OnResetClicked();

	UFUNCTION()
	void OnStartClicked();
};
