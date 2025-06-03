// Copyright 2025 Teyon. All Rights Reserved.


#include "EndRaceWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UEndRaceWidget::NativeConstruct()
{
    Super::NativeConstruct();

    RaceAgainButton->OnClicked.AddDynamic(this, &UEndRaceWidget::OnRaceAgainClicked);
    MenuButton->OnClicked.AddDynamic(this, &UEndRaceWidget::OnMenuClicked);
}


void UEndRaceWidget::OnRaceAgainClicked()
{
    RemoveFromParent();

    UGameplayStatics::OpenLevel(this, "TestMap");
}

void UEndRaceWidget::OnMenuClicked()
{
    RemoveFromParent();

    UGameplayStatics::OpenLevel(this, "TestMap");
}
