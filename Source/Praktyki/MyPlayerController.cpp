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
#include "Components/Image.h"

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

    if (Car)
    {
        Car->OnPenaltyUpdated.AddDynamic(this, &AMyPlayerController::ShowPenalty);
    }
    
}


void AMyPlayerController::Tick(float DeltaTime)
{
    if (GameMode && !GameMode->bIsMenu && RaceWidget)
    {
        if (GameMode->bIsQuali)
        {
            RaceWidget->Timer->SetText(FText::FromString(FormatTime(GameMode->QualiCountdownTime, false)));
        }
        else if (GameMode->bIsRace)
        {
            RaceWidget->Timer->SetText(FText::FromString(FormatTime(GameMode->QualiTime, false)));
        }
        
        RaceWidget->CurrentLap->SetText(FText::FromString(FormatTime(GameMode->QualiTime - Car->StartLapTime, true)));
        if (Car->bStartedFirstLap)
        {
            RaceWidget->SectorTimer->SetText(FText::FromString(FormatTime(GameMode->QualiTime - Car->SectorStartTime, true)));
        }
    }

    if (GameMode && !GameMode->bIsMenu && Car->bRaceEnded && !bEndRaceWidget)
    {
         GetEndRaceStatistics();
         bEndRaceWidget = true;

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

            bShowMouseCursor = true;

            FInputModeUIOnly InputMode;
            InputMode.SetWidgetToFocus(MainMenuWidget->TakeWidget());
            InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            SetInputMode(InputMode);
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
            RaceWidget->Laps->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), Car->StartedLaps, GameMode->LapLimit)));
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
            EndRaceWidget->OwningRacingCar = Car;
            bShowMouseCursor = true;

            FInputModeUIOnly InputMode;
            InputMode.SetWidgetToFocus(EndRaceWidget->TakeWidget());
            InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            SetInputMode(InputMode);
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

void AMyPlayerController::ChangeWidgetsAtRaceStart()
{
    RemoveStartRaceWidget();
    AddRaceWidget();
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
            if (Car->BestQualiLap == 0.0f)
            {
                EndRaceWidget->BestTime->SetText(FText::FromString(TEXT("NO TIME")));
            }
            else
            {
                EndRaceWidget->BestTime->SetText(FText::FromString(FormatTime(Car->BestQualiLap, true)));
            }
            EndRaceWidget->FastestLap->SetText(FText::FromString(FormatTime(Car->BestRaceLap, true)));
            EndRaceWidget->Penalties->SetText(FText::FromString(FString::Printf(TEXT("+%.1f"), Car->Penalty)));
            EndRaceWidget->RaceTime->SetText(FText::FromString(FormatTime(GameMode->QualiTime + Car->Penalty, true)));
        }
    }
}

void AMyPlayerController::SectorUpdate(int Index, float TimerTime)
{
    //ARacingCar* Car = Cast<ARacingCar>(GetPawn());
    //APraktykiGameModeBase* GameMode = Cast<APraktykiGameModeBase>(GetWorld()->GetAuthGameMode());
    /*
    if (GameMode->bIsRace)
    {
        RaceWidget->BestLap->SetText(FText::FromString(FormatTime(Car->BestRaceLap, true)));
        RaceWidget->Laps->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), Car->StartedLaps, GameMode->LapLimit)));
    }
    else if (GameMode->bIsQuali)
    {
        if (Car->StartedLaps != 1 && !Car->bPreviousLapPenalty)
        {
            RaceWidget->BestLap->SetText(FText::FromString(FormatTime(Car->BestQualiLap, true)));
        }
        RaceWidget->Laps->SetText(FText::FromString(FString::Printf(TEXT("%d"), Car->StartedLaps)));
    }
    */
    //RaceWidget->UpdateSectorBox(Index);

    if (Index == 0)
    {
        //Start of Race
        if (Car->PreviousSectorNumber == 0)
        {
            Car->StartLapTime = TimerTime;
            Car->SectorStartTime = TimerTime;
            Car->bStartedFirstLap = true;
            Car->bFirstLap = true;

            RaceWidget->Laps->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), Car->StartedLaps, GameMode->LapLimit)));
        }
        //End of Lap
        else if (Car->PreviousSectorNumber == Car->NumberOfSectors - 1)
        {
            Car->bFirstLap = false;

            //End of lap, so the last sector ends here
            Car->CurrentSectorTimes[Car->NumberOfSectors - 1] = TimerTime - Car->SectorStartTime;

            if (Car->CurrentSectorTimes[Car->NumberOfSectors - 1] < Car->BestSectorTimes[Car->NumberOfSectors - 1])
            {
                if (!Car->bThisLapPenalty)
                {
                    Car->BestSectorTimes[Car->NumberOfSectors - 1] = Car->CurrentSectorTimes[Car->NumberOfSectors - 1];
                }
                
                RaceWidget->SectorImages[Car->NumberOfSectors - 1]->SetColorAndOpacity(FLinearColor::Green);
            }
            else
            {
                RaceWidget->SectorImages[Car->NumberOfSectors - 1]->SetColorAndOpacity(FLinearColor::Yellow);
            }

            
            Car->PreviousLapTime = TimerTime - Car->StartLapTime;
            Car->StartLapTime = TimerTime;
            Car->SectorStartTime = TimerTime;

            if (GameMode->bIsRace)
            {
                if (Car->PreviousLapTime < Car->BestRaceLap || Car->StartedLaps == 1)
                {
                    Car->BestRaceLap = Car->PreviousLapTime;
                }

                Car->StartedLaps += 1;
                RaceWidget->BestLap->SetText(FText::FromString(FormatTime(Car->BestRaceLap, true)));
            }
            else if (GameMode->bIsQuali)
            {
                if (!Car->bThisLapPenalty)
                {
                    if (Car->BestQualiLap > Car->PreviousLapTime)
                    {
                        Car->BestQualiLap = Car->PreviousLapTime;
                        RaceWidget->BestLap->SetText(FText::FromString(FormatTime(Car->BestQualiLap, true)));
                    }
                    else if (Car->BestQualiLap == 0.0f)
                    {
                        Car->BestQualiLap = Car->PreviousLapTime;
                        RaceWidget->BestLap->SetText(FText::FromString(FormatTime(Car->BestQualiLap, true)));
                    }

                }

                Car->StartedLaps += 1;
                RaceWidget->UpdateSectorBox();
            }

            Car->bPreviousLapPenalty = Car->bThisLapPenalty;
            Car->bThisLapPenalty = false;
            Car->PreviousSectorNumber = Index;
        }

    }
    else
    {

        Car->CurrentSectorTimes[Index - 1] = TimerTime - Car->SectorStartTime;

        if (Car->CurrentSectorTimes[Index - 1] < Car->BestSectorTimes[Index - 1])
        {
            if (!Car->bThisLapPenalty)
            {
                Car->BestSectorTimes[Index - 1] = Car->CurrentSectorTimes[Index - 1];
            }
            RaceWidget->SectorImages[Index - 1]->SetColorAndOpacity(FLinearColor::Green);
        }
        else
        {
            RaceWidget->SectorImages[Index - 1]->SetColorAndOpacity(FLinearColor::Yellow);
        }

        Car->SectorStartTime = TimerTime;

        if (Car->PreviousSectorNumber == Index - 1)
        {
            Car->PreviousSectorNumber = Index;
        }
    }

    if (Car->StartedLaps > GameMode->LapLimit && !Car->bRaceEnded && GameMode->bIsRace)
    {
        Car->bRaceEnded = true;
        Car->StopCar();
    }

    if (GameMode->bIsRace)
    {
        RaceWidget->Laps->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), Car->StartedLaps, GameMode->LapLimit)));
    }
    else if (GameMode->bIsQuali)
    {
        RaceWidget->Laps->SetText(FText::FromString(FString::Printf(TEXT("%d"), Car->StartedLaps)));
    }


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

    GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AMyPlayerController::ChangeWidgetsAtRaceStart, 1.0f, false);

}

void AMyPlayerController::ShowPenalty(float Penalty)
{
    FString Msg = FString::Printf(TEXT("PENALTY"));
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, Msg);
    if (RaceWidget)
    {
        Msg = FString::Printf(TEXT("WIDGET"));
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, Msg);
        if (GameMode->bIsRace)
        {
            RaceWidget->PenaltyText->SetText(FText::FromString(TEXT("PENALTY: +5 SECONDS - Exceeded track limits")));
        }
        else if (GameMode->bIsQuali)
        {
            RaceWidget->PenaltyText->SetText(FText::FromString(TEXT("PENALTY: LAP TIME DELETED - Exceeded track limits")));
        }
        RaceWidget->ShowPenalty();
        GetWorld()->GetTimerManager().SetTimer(VisibilityTimer, this, &AMyPlayerController::HidePenalty, 3.0f, false);
    }
}

void AMyPlayerController::HidePenalty()
{
    if (RaceWidget)
    {
        RaceWidget->HidePenalty();
    }
}
