// Copyright 2025 Teyon. All Rights Reserved.


#include "MyPlayerController.h"
#include "MainMenuWidget.h"
#include "RaceWidget.h"
#include "StartRaceWidget.h"
#include "EndRaceWidget.h"
#include "RacingCar.h"
#include <Kismet/GameplayStatics.h>
#include "PraktykiGameModeBase.h"
#include "Components/TextBlock.h"

void AMyPlayerController::BeginPlay()
{
    Super::BeginPlay();

    Car = Cast<ARacingCar>(GetPawn());
    GameMode = Cast<APraktykiGameModeBase>(GetWorld()->GetAuthGameMode());
    if (Car)
    {
        MainMenuWidgetClass = Car->MainMenuWidgetClass;
        RaceWidgetClass = Car->RaceWidgetClass;
        EndRaceWidgetClass = Car->EndRaceWidgetClass;
        StartRaceWidgetClass = Car->StartRaceWidgetClass;
        //Car->StopCar();

    }

    if (GameMode->bIsMenu)
    {
        AddMainMenuWidget();
        if (Car)
        {
            Car->StopCar();
        }
    }
    else
    {
        AddRaceWidget();
        if (Car)
        {
            Car->StartCar();
        }
    }
    
}


void AMyPlayerController::Tick(float DeltaTime)
{
    if (GameMode && !GameMode->bIsMenu && RaceWidget)
    {
        RaceWidget->Timer->SetText(FText::FromString(FormatTime(GameMode->QualiTime, false)));
        if (Car->bStartedFirstLap)
        {
            RaceWidget->SectorTimer->SetText(FText::FromString(FormatTime(GameMode->QualiTime - Car->SectorStartTime, false)));
        }
    }
}

void AMyPlayerController::AddMainMenuWidget()
{
    if (MainMenuWidgetClass)
    {
        MainMenuWidget = CreateWidget<UMainMenuWidget>(this, MainMenuWidgetClass);
        if (MainMenuWidget)
        {
            MainMenuWidget->AddToViewport();
            MainMenuWidget->OwningRacingCar = Car;
        }
    }
}

void AMyPlayerController::AddRaceWidget()
{
    //APraktykiGameModeBase* GameMode = Cast<APraktykiGameModeBase>(GetWorld()->GetAuthGameMode());
    //ARacingCar* Car = Cast<ARacingCar>(GetPawn());
    if (RaceWidgetClass)
    {
        RaceWidget = CreateWidget<URaceWidget>(this, RaceWidgetClass);
        if (RaceWidget)
        {
            RaceWidget->AddToViewport();
            RaceWidget->OwningRacingCar = Car;
            RaceWidget->Laps->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), Car->StartedLaps, GameMode->LapLimit)));
            RaceWidget->CreateSectorBox();
        }
    }
}

void AMyPlayerController::AddStartRaceWidget()
{
    if (StartRaceWidgetClass)
    {
        StartRaceWidget = CreateWidget<UStartRaceWidget>(this, StartRaceWidgetClass);
        if (StartRaceWidget)
        {
            StartRaceWidget->AddToViewport();
        }
    }
}

void AMyPlayerController::AddEndRaceWidget()
{
    if (EndRaceWidgetClass)
    {
        EndRaceWidget = CreateWidget<UEndRaceWidget>(this, EndRaceWidgetClass);
        if (EndRaceWidget)
        {
            EndRaceWidget->AddToViewport();
        }
    }
}

void AMyPlayerController::RemoveMainMenuWidget()
{
    if (MainMenuWidget)
    {
        MainMenuWidget->RemoveFromParent();
    }
}

void AMyPlayerController::RemoveRaceWidget()
{
    if (RaceWidget)
    {
        RaceWidget->RemoveFromParent();
    }
}

void AMyPlayerController::RemoveStartRaceWidget()
{
    if (StartRaceWidget)
    {
        StartRaceWidget->RemoveFromParent();
    }
}

void AMyPlayerController::RemoveEndRaceWidget()
{
    if (EndRaceWidget)
    {
        EndRaceWidget->RemoveFromParent();
    }
}

void AMyPlayerController::UpdateRaceWidget()
{

}


void AMyPlayerController::GetEndRaceStatistics()
{
    RemoveRaceWidget();
    AddEndRaceWidget();

    //ARacingCar* Car = Cast<ARacingCar>(GetPawn());
    //APraktykiGameModeBase* GameMode = Cast<APraktykiGameModeBase>(GetWorld()->GetAuthGameMode());
    
    if (EndRaceWidgetClass)
    {
        if (EndRaceWidget)
        {
            bShowMouseCursor = true;

            EndRaceWidget->BestTime->SetText(FText::FromString(FormatTime(Car->BestQualiLap, true)));
            EndRaceWidget->FastestLap->SetText(FText::FromString(FormatTime(Car->BestRaceLap, true)));
            EndRaceWidget->Penalties->SetText(FText::FromString(FormatTime(Car->Penalty, true)));
            EndRaceWidget->RaceTime->SetText(FText::FromString(FormatTime(GameMode->QualiTime, true)));
        }
    }
}

void AMyPlayerController::SectorUpdate(int Index)
{
    //ARacingCar* Car = Cast<ARacingCar>(GetPawn());
    //APraktykiGameModeBase* GameMode = Cast<APraktykiGameModeBase>(GetWorld()->GetAuthGameMode());
    if (GameMode->bIsRace)
    {
        RaceWidget->BestLap->SetText(FText::FromString(FormatTime(Car->BestRaceLap, true)));
        RaceWidget->Laps->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), Car->StartedLaps, GameMode->LapLimit)));
    }
    else if (GameMode->bIsQuali)
    {
        RaceWidget->BestLap->SetText(FText::FromString(FormatTime(Car->BestQualiLap, true)));
        RaceWidget->Laps->SetText(FText::FromString(FString::Printf(TEXT("%d"), Car->StartedLaps)));
    }

    RaceWidget->UpdateSectorBox(Index);

}

FString AMyPlayerController::FormatTime(float TimeSeconds, bool bMilliseconds)
{
    int32 TotalMilliseconds = FMath::RoundToInt(TimeSeconds * 1000.0f);
    int32 Minutes = TotalMilliseconds / 60000;
    int32 Seconds = (TotalMilliseconds % 60000) / 1000;
    if (bMilliseconds)
    {
        int32 Milliseconds = TotalMilliseconds % 1000;
        return FString::Printf(TEXT("%01d:%02d:%03d"), Minutes, Seconds, Milliseconds);
    }

    return FString::Printf(TEXT("%01d:%02d"), Minutes, Seconds);
}

void AMyPlayerController::StartRaceCountdown()
{
    RaceWidget->SetVisibility(ESlateVisibility::Hidden);

    AddStartRaceWidget();
}

void AMyPlayerController::LightOn(int LightIndex)
{
    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("LIGHT ON"));
    StartRaceWidget->SetLightColour(LightIndex, FLinearColor::Red);
}

void AMyPlayerController::LightsOut()
{
    StartRaceWidget->SetLightColour(0, FLinearColor::Black);
    StartRaceWidget->SetLightColour(1, FLinearColor::Black);
    StartRaceWidget->SetLightColour(2, FLinearColor::Black);
    StartRaceWidget->SetLightColour(3, FLinearColor::Black);
    StartRaceWidget->SetLightColour(4, FLinearColor::Black);



}