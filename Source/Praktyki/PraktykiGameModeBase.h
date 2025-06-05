// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PraktykiGameModeBase.generated.h"

class ARacingCar;
class ATimingLine;
class AMyPlayerController;
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
    float MaxQualiTime = 200.f;
    float QualiCountdownTime = 300.0f;
    bool bQualiOver = false;

    int LapLimit = 1;
    int CurrentLap = 1;
    bool bRaceOver = false;
    float RaceTime = 0.0f;

    bool bIsQuali = false;
    bool bIsRace = false;
    bool bIsGhostCar = false;

    int NrOfSectors = 0;
 
    int CurrentLight = 0;
    int LightsNumber = 5;

    void QualiStart();
    void QualiEnd();

    void RaceStart();
    void RaceEnd();

    void LightsOut();

    void LightSequence();

    void GetInstanceValues();

    FTimerHandle TimerHandleTimeToStart;

    int SectorNumber = 0;

    bool bIsMenu = false;
	 
};
