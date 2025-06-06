// Copyright 2025 Teyon. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

class UMainMenuWidget;
class URaceWidget;
class UStartRaceWidget;
class UEndRaceWidget;
class ARacingCar;
class APraktykiGameModeBase;

/**
 * 
 */
UCLASS()
class PRAKTYKI_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()


protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    UPROPERTY()
    URaceWidget* RaceWidget;

    UPROPERTY()
    UStartRaceWidget* StartRaceWidget;

    UPROPERTY()
    UEndRaceWidget* EndRaceWidget;

    UPROPERTY()
    UMainMenuWidget* MainMenuWidget;

    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<URaceWidget> RaceWidgetClass;

    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UStartRaceWidget> StartRaceWidgetClass;

    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UEndRaceWidget> EndRaceWidgetClass;

    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UMainMenuWidget> MainMenuWidgetClass;

    UPROPERTY()
    APraktykiGameModeBase* GameMode;

    UPROPERTY()
    ARacingCar* Car;

    FTimerHandle TimerHandle;

    FTimerHandle VisibilityTimer;

    bool bEndRaceWidget = false;

    void AddMainMenuWidget();

    void AddRaceWidget();

    void AddStartRaceWidget();

    void AddEndRaceWidget();

    void RemoveMainMenuWidget();

    void RemoveRaceWidget();

    void RemoveStartRaceWidget();

    void RemoveEndRaceWidget();

    void UpdateRaceWidget();

    void ChangeWidgetsAtRaceStart();

    void GetEndRaceStatistics();

    void SectorUpdate(int Index, float TimerTime);

    FString FormatTime(float TimeSeconds, bool bMilliseconds);

    void StartRaceCountdown();

    void LightOn(int LightIndex);

    void LightsOut();

    UFUNCTION()
    void ShowPenalty(float Penalty);

    void HidePenalty();

    void ResetForRace();

private:

    TArray<float> CurrentSectorTimes;
    TArray<float> PreviousBestSectorTimes;
    TArray<float> BestSectorTimes;
    TArray<float> LapTimes;

    int NumberOfSectors = 0;
    float PreviousLapTime;
    float BestRaceLap;
    float BestQualiLap = 0.0f;
    float StartLapTime;
    float SectorStartTime;
    float RaceTime;
    int PreviousSectorNumber = 0;
    int StartedLaps = 1;
    bool bRaceEnded = false;
    bool bStartedFirstLap = false;
    bool bFirstLap = false;

	
};
