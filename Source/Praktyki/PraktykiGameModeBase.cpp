// Fill out your copyright notice in the Description page of Project Settings.


#include "PraktykiGameModeBase.h"
#include "RacingCar.h"
#include "TimingLine.h"
#include <Kismet/GameplayStatics.h>
#include "MyPlayerController.h"
#include "RacingGameInstance.h"

APraktykiGameModeBase::APraktykiGameModeBase()
{
    PrimaryActorTick.bCanEverTick = true;
    PlayerControllerClass = AMyPlayerController::StaticClass();
}

void APraktykiGameModeBase::BeginPlay()
{
    Super::BeginPlay();
    QualiTime = 0.f;
    QualiCountdownTime = 300.0f;
    bIsQuali = false;
    TArray<AActor*> AllTimingLines;

    FString MapName = GetWorld()->GetMapName();
    MapName = FPackageName::GetShortName(MapName);

    if (MapName.Contains(TEXT("Menu")))
    {
        bIsMenu = true;
    }
    else
    {
        GetInstanceValues();
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATimingLine::StaticClass(), AllTimingLines);
        SectorNumber = AllTimingLines.Num();
        bIsMenu = false;

        QualiCountdownTime = MaxQualiTime;
        QualiStart();
    }
    
}

void APraktykiGameModeBase::QualiStart()
{

    bIsQuali = true;
    ARacingCar* Car = Cast<ARacingCar>(GetWorld()->GetFirstPlayerController()->GetPawn());
    if (Car)
    {
        Car->StartCar();
    }
}

void APraktykiGameModeBase::QualiEnd()
{
    ARacingCar* Car = Cast<ARacingCar>(GetWorld()->GetFirstPlayerController()->GetPawn());
    QualiTime = 0.0f;
    QualiCountdownTime = 300.0f;
    if (Car)
    {
        AMyPlayerController* PC = Cast<AMyPlayerController>(Car->GetController());
        if (PC)
        {
            PC->ResetForRace();
            Car->PrepareForRace();
            PC->StartRaceCountdown();
        }

    }

    CurrentLight = 0;
    GetWorld()->GetTimerManager().SetTimer(TimerHandleTimeToStart, this, &APraktykiGameModeBase::LightSequence, 3.0f, false);
}

void APraktykiGameModeBase::RaceStart()
{
    bIsRace = true;
    ARacingCar* Car = Cast<ARacingCar>(GetWorld()->GetFirstPlayerController()->GetPawn());
    if (Car)
    {
        Car->StartCar();
    }
}

void APraktykiGameModeBase::LightsOut()
{
    ARacingCar* Car = Cast<ARacingCar>(GetWorld()->GetFirstPlayerController()->GetPawn());
    if (Car)
    {
        AMyPlayerController* PC = Cast<AMyPlayerController>(Car->GetController());
        if (PC)
        {
            PC->LightsOut();
        }
    }
    RaceStart();
}

void APraktykiGameModeBase::LightSequence()
{
    ARacingCar* Car = Cast<ARacingCar>(GetWorld()->GetFirstPlayerController()->GetPawn());
    if (Car)
    {

        AMyPlayerController* PC = Cast<AMyPlayerController>(Car->GetController());
        if (PC)
        {
            PC->LightOn(CurrentLight);
        }

        CurrentLight++;
        if (CurrentLight < LightsNumber)
        {
            GetWorld()->GetTimerManager().SetTimer(TimerHandleTimeToStart, this, &APraktykiGameModeBase::LightSequence, 1.0f, false);
        }
        else
        {
            float RandomDelay = FMath::FRandRange(0.2f, 1.5f);
            GetWorld()->GetTimerManager().SetTimer(TimerHandleTimeToStart, this, &APraktykiGameModeBase::LightsOut, RandomDelay, false);
        }
    }
}

void APraktykiGameModeBase::GetInstanceValues()
{

    UGameInstance* GI = GetWorld()->GetGameInstance();
    URacingGameInstance* GameInstance = Cast<URacingGameInstance>(GI);

    ARacingCar* Car = Cast<ARacingCar>(GetWorld()->GetFirstPlayerController()->GetPawn());
    if (Car)
    {
        MaxQualiTime = GameInstance->TimeLimitValue;
        LapLimit = GameInstance->LapLimitValue;
        bIsGhostCar = GameInstance->bGhostCar;
    }
}

void APraktykiGameModeBase::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (bIsQuali)
    {
        QualiCountdownTime -= DeltaSeconds;
        QualiTime += DeltaSeconds;

        if (QualiCountdownTime <= 0.f)
        {

            QualiCountdownTime = 0.f;

            ARacingCar* Car = Cast<ARacingCar>(GetWorld()->GetFirstPlayerController()->GetPawn());
            if (Car)
            {
                Car->StopCar();
            }

            bIsQuali = false;
            QualiEnd();

        }
    }
    else if (bIsRace)
    {
        QualiTime += DeltaSeconds;
    }
}