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

    void AddMainMenuWidget();

    void AddRaceWidget();

    void AddStartRaceWidget();

    void AddEndRaceWidget();

    void RemoveMainMenuWidget();

    void RemoveRaceWidget();

    void RemoveStartRaceWidget();

    void RemoveEndRaceWidget();

    void UpdateRaceWidget();

    void GetEndRaceStatistics();

    void SectorUpdate(int Index);

    FString FormatTime(float TimeSeconds, bool bMilliseconds);

    void StartRaceCountdown();

    void LightOn(int LightIndex);

    void LightsOut();

	
};
