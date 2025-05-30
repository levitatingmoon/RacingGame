// Fill out your copyright notice in the Description page of Project Settings.


#include "PraktykiGameModeBase.h"
#include "RacingCar.h"
#include "TimingLine.h"
#include <Kismet/GameplayStatics.h>

APraktykiGameModeBase::APraktykiGameModeBase()
{
    PrimaryActorTick.bCanEverTick = true;
}

void APraktykiGameModeBase::BeginPlay()
{
    Super::BeginPlay();
    QualiTime = 0.f;
    bQualiOver = false;
    bIsQuali = false;

    UE_LOG(LogTemp, Warning, TEXT("Race Starts: %.2f"), QualiTime);

    GetWorld()->GetTimerManager().SetTimer(TimerHandleTimeToStart, this, &APraktykiGameModeBase::QualiStart, 3.0f, false);

    TArray<AActor*> AllTimingLines;
    if (GetWorld())
    {
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATimingLine::StaticClass(), AllTimingLines);
    }

    SectorNumber = AllTimingLines.Num();
}

void APraktykiGameModeBase::QualiStart()
{
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("QUALI START"));
    bIsQuali = true;
}

void APraktykiGameModeBase::QualiEnd()
{
    GetWorld()->GetTimerManager().SetTimer(TimerHandleTimeToStart, this, &APraktykiGameModeBase::RaceStart, 3.0f, false);
}

void APraktykiGameModeBase::RaceStart()
{
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("RACE START"));
    bIsRace = true;
    ARacingCar* Car = Cast<ARacingCar>(GetWorld()->GetFirstPlayerController()->GetPawn());
    if (Car)
    {
        Car->StartCar();
    }
    QualiTime = 0.0f;
}

void APraktykiGameModeBase::RaceEnd()
{
}

void APraktykiGameModeBase::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (bIsQuali)
    {
        QualiTime += DeltaSeconds;

        if (QualiTime >= MaxQualiTime)
        {
            bQualiOver = true;
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("QUALI END"));

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