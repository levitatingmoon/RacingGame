// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RacingCar.h"
#include "PraktykiGameModeBase.generated.h"

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

    int LapLimit = 30;
    int CurrentLap = 1;
    bool bRaceOver = false;

    bool bIsQuali = false;
    bool bIsRace = false;

    void QualiStart();
    void QualiEnd();

    void RaceStart();
    void RaceEnd();

    FTimerHandle TimerHandleTimeToStart;
	 
};
