// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PraktykiGameModeBase.generated.h"

class ARacingCar;
class ATimingLine;
/**
 * 
 */
UCLASS()
class PRAKTYKI_API APraktykiGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

    APraktykiGameModeBase();

    virtual void Tick(float DeltaSeconds) override;

protected:
    virtual void BeginPlay() override;

public:
    float QualiTime = 0.f;
    float MaxQualiTime = 10.f;
    bool bQualiOver = false;

    int LapLimit = 1;
    int CurrentLap = 1;
    bool bRaceOver = false;
    float RaceTime = 0.f;

    bool bIsQuali = false;
    bool bIsRace = false;

    int NrOfSectors = 0;

    void QualiStart();
    void QualiEnd();

    void RaceStart();
    void RaceEnd();

    FTimerHandle TimerHandleTimeToStart;

    int SectorNumber = 0;
	 
};
