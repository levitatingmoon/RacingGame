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
#include "Components/AudioComponent.h"
#include <Components/CanvasPanelSlot.h>
#include "GhostCar.h"

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

    }

    NumberOfSectors = GameMode->SectorNumber;
    CurrentSectorTimes.SetNum(NumberOfSectors);
    BestSectorTimes.SetNum(NumberOfSectors);
    PreviousBestSectorTimes.SetNum(NumberOfSectors);
    bChangedBestTimes.SetNum(NumberOfSectors);

    for (int i = 0; i < NumberOfSectors; i++)
    {
        BestSectorTimes[i] = FLT_MAX;
        PreviousBestSectorTimes[i] = FLT_MAX;
        CurrentSectorTimes[i] = 0.0f;
        bChangedBestTimes[i] = false;
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

        RaceWidget->Speed->SetText(FText::FromString(FString::Printf(TEXT("%.0f km/h"), Car->CarSkeletalMesh->GetComponentVelocity().Size() * 0.036f)));
        RaceWidget->CurrentLap->SetText(FText::FromString(FormatTime(GameMode->QualiTime - StartLapTime, true)));
        if (bStartedFirstLap)
        {
            RaceWidget->SectorTimer->SetText(FText::FromString(FormatTime(GameMode->QualiTime - SectorStartTime, true)));
        }
    }

    if (GameMode && !GameMode->bIsMenu && bRaceEnded && !bEndRaceWidget)
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

    if (RaceWidgetClass)
    {
        RaceWidget = CreateWidget<URaceWidget>(this, RaceWidgetClass);
        if (RaceWidget)
        {
            RaceWidget->AddToViewport();
            RaceWidget->OwningRacingCar = Car;
            RaceWidget->Laps->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), StartedLaps, GameMode->LapLimit)));
            RaceWidget->CreateSectorBox();

            if (GameMode->bIsRace)
            {
                RaceWidget->PenaltyValue->SetVisibility(ESlateVisibility::Visible);
                RaceWidget->PenaltyValueText->SetVisibility(ESlateVisibility::Visible);
                UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(RaceWidget->BackgroundData->Slot);
                if (CanvasSlot)
                {
                    CanvasSlot->SetSize(FVector2D(580.f, 335.f));
                }
            }
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

void AMyPlayerController::ChangeWidgetsAtRaceStart()
{
    RemoveStartRaceWidget();
    AddRaceWidget();
}


void AMyPlayerController::GetEndRaceStatistics()
{
    RemoveRaceWidget();
    AddEndRaceWidget();
    Car->EngineSound->Stop();

    if (Car->LastGhost)
    {
        Car->LastGhost->Destroy();
    }
    
    if (EndRaceWidgetClass)
    {
        if (EndRaceWidget)
        {
            if (BestQualiLap == 0.0f)
            {
                EndRaceWidget->BestTime->SetText(FText::FromString(TEXT("NO TIME")));
            }
            else
            {
                EndRaceWidget->BestTime->SetText(FText::FromString(FormatTime(BestQualiLap, true)));
            }
            EndRaceWidget->FastestLap->SetText(FText::FromString(FormatTime(BestRaceLap, true)));
            EndRaceWidget->Penalties->SetText(FText::FromString(FString::Printf(TEXT("+%.1f"), Car->Penalty)));
            EndRaceWidget->RaceTime->SetText(FText::FromString(FormatTime(GameMode->QualiTime, true)));
            EndRaceWidget->RaceFinalTime->SetText(FText::FromString(FormatTime(GameMode->QualiTime + Car->Penalty, true)));
        }
    }
}

void AMyPlayerController::SectorUpdate(int Index, float TimerTime)
{

    if (Index == 0)
    {
        //Start of Race
        if (PreviousSectorNumber == 0)
        {
            StartLapTime = TimerTime;
            SectorStartTime = TimerTime;
            bStartedFirstLap = true;

            if (GameMode->bIsGhostCar)
            {
                Car->StartGhostRecording();
            }
            

            RaceWidget->Laps->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), StartedLaps, GameMode->LapLimit)));
        }
        //End of Lap
        else if (PreviousSectorNumber == NumberOfSectors - 1)
        {
            if (GameMode->bIsGhostCar)
            {
                Car->OnLapCompleted();
            }

            //End of lap, so the last sector ends here
            CurrentSectorTimes[NumberOfSectors - 1] = TimerTime - SectorStartTime;
            
            if (CurrentSectorTimes[NumberOfSectors - 1] < BestSectorTimes[NumberOfSectors - 1])
            {

                if (BestSectorTimes[NumberOfSectors - 1] != FLT_MAX)
                {
                    float TimeDifference = CurrentSectorTimes[NumberOfSectors - 1] - BestSectorTimes[NumberOfSectors - 1];
                    FString TimeDifferenceString = FormatTime(FMath::Abs(TimeDifference), true);
                    if (TimeDifference < 0.0f)
                    {
                        RaceWidget->SectorTimeDifference->SetColorAndOpacity(FLinearColor::Green);
                        RaceWidget->SectorTimeDifference->SetText(FText::FromString(FString::Printf(TEXT("-%s"), *TimeDifferenceString)));
                    }
                    else
                    {
                        RaceWidget->SectorTimeDifference->SetColorAndOpacity(FLinearColor::Yellow);
                        RaceWidget->SectorTimeDifference->SetText(FText::FromString(FString::Printf(TEXT("+%s"), *TimeDifferenceString)));
                    }
                    RaceWidget->ShowSectorDifference();
                }

                if (!Car->bThisLapPenalty)
                {
                    PreviousBestSectorTimes[NumberOfSectors - 1] = BestSectorTimes[NumberOfSectors - 1];
                    BestSectorTimes[NumberOfSectors - 1] = CurrentSectorTimes[NumberOfSectors - 1];
                    bChangedBestTimes[NumberOfSectors - 1] = true;
                }
                
                RaceWidget->SectorImages[NumberOfSectors - 1]->SetColorAndOpacity(FLinearColor::Green);
            }
            else
            {

                if (BestSectorTimes[NumberOfSectors - 1] != FLT_MAX)
                {
                    float TimeDifference = CurrentSectorTimes[NumberOfSectors - 1] - BestSectorTimes[NumberOfSectors - 1];
                    FString TimeDifferenceString = FormatTime(FMath::Abs(TimeDifference), true);
                    if (TimeDifference < 0.0f)
                    {
                        RaceWidget->SectorTimeDifference->SetColorAndOpacity(FLinearColor::Green);
                        RaceWidget->SectorTimeDifference->SetText(FText::FromString(FString::Printf(TEXT("-%s"), *TimeDifferenceString)));
                    }
                    else
                    {
                        RaceWidget->SectorTimeDifference->SetColorAndOpacity(FLinearColor::Yellow);
                        RaceWidget->SectorTimeDifference->SetText(FText::FromString(FString::Printf(TEXT("+%s"), *TimeDifferenceString)));
                    }
                    RaceWidget->ShowSectorDifference();
                }

                RaceWidget->SectorImages[NumberOfSectors - 1]->SetColorAndOpacity(FLinearColor::Yellow);
            }

            
            PreviousLapTime = TimerTime - StartLapTime;
            RaceWidget->PreviousLap->SetText(FText::FromString(FormatTime(PreviousLapTime, true)));
            if (!Car->bThisLapPenalty)
            {
                RaceWidget->PreviousLap->SetColorAndOpacity(FLinearColor::White);
            }
            else
            {
                RaceWidget->PreviousLap->SetColorAndOpacity(FLinearColor::Red);
            }

            StartLapTime = TimerTime;
            SectorStartTime = TimerTime;

            if (GameMode->bIsRace)
            {
                if (PreviousLapTime < BestRaceLap || StartedLaps == 1)
                {
                    BestRaceLap = PreviousLapTime;
                }

                StartedLaps += 1;
                RaceWidget->BestLap->SetText(FText::FromString(FormatTime(BestRaceLap, true)));
            }
            else if (GameMode->bIsQuali)
            {
                if (!Car->bThisLapPenalty)
                {
                    if (BestQualiLap > PreviousLapTime)
                    {
                        BestQualiLap = PreviousLapTime;
                        RaceWidget->BestLap->SetText(FText::FromString(FormatTime(BestQualiLap, true)));
                    }
                    else if (BestQualiLap == 0.0f)
                    {
                        BestQualiLap = PreviousLapTime;
                        RaceWidget->BestLap->SetText(FText::FromString(FormatTime(BestQualiLap, true)));
                    }

                }

                StartedLaps += 1;
                RaceWidget->CurrentLap->SetColorAndOpacity(FLinearColor::White);
            }

            if (Car->bThisLapPenalty)
            {
                for (int i = 0; i < NumberOfSectors; i++)
                {
                    if (bChangedBestTimes[i] == true)
                    {
                        BestSectorTimes[i] = PreviousBestSectorTimes[i];
                    }
                }
            }

            Car->bThisLapPenalty = false;
            PreviousSectorNumber = Index;

            for (int i = 0; i < NumberOfSectors; i++)
            {
                bChangedBestTimes[i] = false;
            }
            RaceWidget->UpdateSectorBox();
        }

    }
    else
    {

        CurrentSectorTimes[Index - 1] = TimerTime - SectorStartTime;

        if (CurrentSectorTimes[Index - 1] < BestSectorTimes[Index - 1])
        {
            
            if (BestSectorTimes[Index - 1] != FLT_MAX)
            {
                float TimeDifference = CurrentSectorTimes[Index - 1] - BestSectorTimes[Index - 1];
                FString TimeDifferenceString = FormatTime(FMath::Abs(TimeDifference), true);
                if (TimeDifference < 0.0f)
                {
                    RaceWidget->SectorTimeDifference->SetColorAndOpacity(FLinearColor::Green);
                    RaceWidget->SectorTimeDifference->SetText(FText::FromString(FString::Printf(TEXT("-%s"), *TimeDifferenceString)));
                }
                else
                {
                    RaceWidget->SectorTimeDifference->SetColorAndOpacity(FLinearColor::Yellow);
                    RaceWidget->SectorTimeDifference->SetText(FText::FromString(FString::Printf(TEXT("+%s"), *TimeDifferenceString)));
                }
                RaceWidget->ShowSectorDifference();
            }


            if (!Car->bThisLapPenalty)
            {
                PreviousBestSectorTimes[Index - 1] = BestSectorTimes[Index - 1];
                BestSectorTimes[Index - 1] = CurrentSectorTimes[Index - 1];
                bChangedBestTimes[Index - 1] = true;
            }

            RaceWidget->SectorImages[Index - 1]->SetColorAndOpacity(FLinearColor::Green);
        }
        else
        {
            if (BestSectorTimes[Index - 1] != FLT_MAX)
            {
                float TimeDifference = CurrentSectorTimes[Index - 1] - BestSectorTimes[Index - 1];
                FString TimeDifferenceString = FormatTime(FMath::Abs(TimeDifference), true);
                if (TimeDifference < 0.0f)
                {
                    RaceWidget->SectorTimeDifference->SetColorAndOpacity(FLinearColor::Green);
                    RaceWidget->SectorTimeDifference->SetText(FText::FromString(FString::Printf(TEXT("-%s"), *TimeDifferenceString)));
                }
                else
                {
                    RaceWidget->SectorTimeDifference->SetColorAndOpacity(FLinearColor::Yellow);
                    RaceWidget->SectorTimeDifference->SetText(FText::FromString(FString::Printf(TEXT("+%s"), *TimeDifferenceString)));
                }
                RaceWidget->ShowSectorDifference();
            }
            RaceWidget->SectorImages[Index - 1]->SetColorAndOpacity(FLinearColor::Yellow);
        }

        SectorStartTime = TimerTime;

        if (PreviousSectorNumber == Index - 1)
        {
            PreviousSectorNumber = Index;
        }
    }
    if (StartedLaps == GameMode->LapLimit && GameMode->bIsGhostCar)
    {
        Car->StopGhostRecording();
    }

    if (StartedLaps > GameMode->LapLimit && !bRaceEnded && GameMode->bIsRace)
    {
        bRaceEnded = true;
        Car->StopCar();
    }

    if (GameMode->bIsRace)
    {
        RaceWidget->Laps->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), StartedLaps, GameMode->LapLimit)));
    }
    else if (GameMode->bIsQuali)
    {
        RaceWidget->Laps->SetText(FText::FromString(FString::Printf(TEXT("%d"), StartedLaps)));
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
    if (StartRaceWidget->LightOnSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, StartRaceWidget->LightOnSound, Car->GetActorLocation());
    }
    StartRaceWidget->SetLightColour(LightIndex, FLinearColor::Red);
}

void AMyPlayerController::LightsOut()
{
    if (StartRaceWidget->LightsOffSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, StartRaceWidget->LightsOffSound, Car->GetActorLocation());
    }
    StartRaceWidget->SetLightColour(0, FLinearColor::Black);
    StartRaceWidget->SetLightColour(1, FLinearColor::Black);
    StartRaceWidget->SetLightColour(2, FLinearColor::Black);
    StartRaceWidget->SetLightColour(3, FLinearColor::Black);
    StartRaceWidget->SetLightColour(4, FLinearColor::Black);

    GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AMyPlayerController::ChangeWidgetsAtRaceStart, 1.0f, false);

}

void AMyPlayerController::ShowPenalty(float Penalty)
{
    
    if (RaceWidget)
    {
       
        if (GameMode->bIsRace)
        {
            RaceWidget->PenaltyText->SetText(FText::FromString(TEXT("PENALTY: +5 SECONDS - Exceeded track limits")));
            RaceWidget->PenaltyValue->SetText(FText::FromString(FString::Printf(TEXT("+%.1f"), Car->Penalty)));
        }
        else if (GameMode->bIsQuali)
        {
            RaceWidget->PenaltyText->SetText(FText::FromString(TEXT("PENALTY: LAP TIME DELETED - Exceeded track limits")));
        }
        RaceWidget->ShowPenalty();
        RaceWidget->CurrentLap->SetColorAndOpacity(FLinearColor::Red);
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

void AMyPlayerController::ResetForRace()
{
    Car->EngineSound->Stop();
    StartedLaps = 1;
    SectorStartTime = 0;
    PreviousSectorNumber = 0;
    PreviousLapTime = 0;
    StartLapTime = 0;

}
