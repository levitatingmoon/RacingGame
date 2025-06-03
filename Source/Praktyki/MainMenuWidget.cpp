// Copyright 2025 Teyon. All Rights Reserved.


#include "MainMenuWidget.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "RacingGameInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

void UMainMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (TimeLimitSlider)
    {
        TimeLimitSlider->OnValueChanged.AddDynamic(this, &UMainMenuWidget::OnTimeLimitSliderChanged);
    }

    if (LapLimitSlider)
    {
        LapLimitSlider->OnValueChanged.AddDynamic(this, &UMainMenuWidget::OnLapLimitSliderChanged);
    }

    if (QuitGameButton)
    {
        QuitGameButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnQuitClicked);
    }

    if (ResetSettingsButton)
    {
        ResetSettingsButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnResetClicked);
    }

    if (StartGameButton)
    {
        StartGameButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnStartClicked);
    }
}

void UMainMenuWidget::OnTimeLimitSliderChanged(float Value)
{
    if (URacingGameInstance* GameInstance = Cast<URacingGameInstance>(GetWorld()->GetGameInstance()))
    {
        GameInstance->TimeLimitValue = Value;
    }

    TimeLimitSliderValue->SetText(FText::AsNumber(FMath::RoundToInt(Value)));
}

void UMainMenuWidget::OnLapLimitSliderChanged(float Value)
{
    if (URacingGameInstance* GameInstance = Cast<URacingGameInstance>(GetWorld()->GetGameInstance()))
    {
        GameInstance->LapLimitValue = Value;
    }

    LapLimitSliderValue->SetText(FText::AsNumber(FMath::RoundToInt(Value)));
}

void UMainMenuWidget::OnQuitClicked()
{
    UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, true);
}

void UMainMenuWidget::OnResetClicked()
{
    TimeLimitSlider->SetValue(TimeLimitSliderDefault);
    OnTimeLimitSliderChanged(TimeLimitSliderDefault);

    LapLimitSlider->SetValue(LapLimitSliderDefault);
    OnTimeLimitSliderChanged(LapLimitSliderDefault);
}

void UMainMenuWidget::OnStartClicked()
{
    RemoveFromParent();

    UGameplayStatics::OpenLevel(this, "TestMap");
}