// Copyright 2025 Teyon. All Rights Reserved.


#include "MainMenuWidget.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "RacingGameInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "RacingCar.h"

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

    if (DefaultMaterialButton)
    {
        DefaultMaterialButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnDefaultClicked);
    }

    if (RedMaterialButton)
    {
        RedMaterialButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnRedClicked);
    }

    if (BlueMaterialButton)
    {
        BlueMaterialButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnBlueClicked);
    }

    if (GreenMaterialButton)
    {
        GreenMaterialButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnGreenClicked);
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
    OnLapLimitSliderChanged(LapLimitSliderDefault);

    OwningRacingCar->ChangeMeshMaterial(3);
}

void UMainMenuWidget::OnStartClicked()
{
    RemoveFromParent();

    APlayerController* PC = Cast<APlayerController>(OwningRacingCar->GetController());
    if (PC)
    {
        PC->bShowMouseCursor = false;
        FInputModeGameOnly InputMode;
        PC->SetInputMode(InputMode);
    }

    UGameplayStatics::OpenLevel(this, "TestMap");
}

void UMainMenuWidget::OnRedClicked()
{
    OwningRacingCar->ChangeMeshMaterial(0);
    if (URacingGameInstance* GameInstance = Cast<URacingGameInstance>(GetWorld()->GetGameInstance()))
    {
        GameInstance->MaterialIndex = 0;
    }
}

void UMainMenuWidget::OnGreenClicked()
{
    OwningRacingCar->ChangeMeshMaterial(1);
    if (URacingGameInstance* GameInstance = Cast<URacingGameInstance>(GetWorld()->GetGameInstance()))
    {
        GameInstance->MaterialIndex = 1;
    }
}

void UMainMenuWidget::OnBlueClicked()
{
    OwningRacingCar->ChangeMeshMaterial(2);
    if (URacingGameInstance* GameInstance = Cast<URacingGameInstance>(GetWorld()->GetGameInstance()))
    {
        GameInstance->MaterialIndex = 2;
    }
}

void UMainMenuWidget::OnDefaultClicked()
{
    OwningRacingCar->ChangeMeshMaterial(3);
    if (URacingGameInstance* GameInstance = Cast<URacingGameInstance>(GetWorld()->GetGameInstance()))
    {
        GameInstance->MaterialIndex = 3;
    }
}
