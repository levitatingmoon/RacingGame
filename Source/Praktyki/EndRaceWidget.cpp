// Copyright 2025 Teyon. All Rights Reserved.


#include "EndRaceWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "RacingCar.h"

void UEndRaceWidget::NativeConstruct()
{
    Super::NativeConstruct();

    RaceAgainButton->OnClicked.AddDynamic(this, &UEndRaceWidget::OnRaceAgainClicked);
    MenuButton->OnClicked.AddDynamic(this, &UEndRaceWidget::OnMenuClicked);
}


void UEndRaceWidget::OnRaceAgainClicked()
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

void UEndRaceWidget::OnMenuClicked()
{
    RemoveFromParent();

    UGameplayStatics::OpenLevel(this, "MenuMap");
}
