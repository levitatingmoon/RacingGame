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

private:

    float MaxQualiTime = 300.f;
    int CurrentLight = 0;
    int LightsNumber = 5;

    void QualiStart();
    void QualiEnd();
    void RaceStart();
    void LightsOut();
    void LightSequence();
    void GetInstanceValues();

    FTimerHandle TimerHandleTimeToStart;

public:

    float QualiTime = 0.f;
    int LapLimit = 1;

    float QualiCountdownTime = 300.0f;

    bool bIsQuali = false;
    bool bIsRace = false;
    bool bIsGhostCar = false;

    int SectorNumber = 0;

    bool bIsMenu = false;
	 
};
