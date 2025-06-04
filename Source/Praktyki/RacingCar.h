#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "RacingCar.generated.h"

class APraktykiGameModeBase;
class URaceWidget;
class UEndRaceWidget;
class UStartRaceWidget;
class UMainMenuWidget;
class ARacingCarMovementComponent;
class AMyPlayerController;
class AStartingSpot;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPenaltyUpdated, float, Penalty);

UCLASS()
class PRAKTYKI_API ARacingCar : public APawn
{
    GENERATED_BODY()

public:
    ARacingCar();

protected:
    virtual void BeginPlay() override;

private:

    TArray<FName> WheelBones = 
    {
        FName("wheel_front_right_spin"),
        FName("wheel_front_left_spin"),
        FName("wheel_back_right_spin"),
        FName("wheel_back_left_spin")
    };

public:
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    FString FormatTime(float TimeSeconds, bool bMilliseconds);
    
    void SuspensionWheelForce();
    void SteerForce();

    UPROPERTY(EditAnywhere, Category = "Physics")
    float GripFactor = 1.0f;
    UPROPERTY(EditAnywhere, Category = "Physics")
    float TireMass = 1.0f;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnPenaltyUpdated OnPenaltyUpdated;


    void Throttle(float Val);
    void Steer(float Val);
    void StopCar();
    void StartCar();
    void PrepareForRace();

    void UseBehindCamera();
    void UseInsideCamera();
    void UseHoodCamera();
    UCameraComponent* FindCameraByName(FName CameraName);

    void StoreCheckpointTime(int SectorNumber, float QualiTime);

    TArray<float> CurrentSectorTimes;
    TArray<float> BestSectorTimes;
    TArray<float> LapTimes;

    int NumberOfSectors = 0;
    float PreviousLapTime;
    float BestRaceLap;
    float BestQualiLap = 10000.0f;
    float StartLapTime;
    float SectorStartTime;
    float RaceTime;
    float Penalty;
    int PreviousSectorNumber = 0;
    int StartedLaps = 1;
    bool bRaceEnded = false;
    bool bStartedFirstLap = false;
    bool bFirstLap = false;

    bool bWasOffTrack = false;
    bool bThisLapPenalty = false;
    bool bPreviousLapPenalty = false;

    FVector2D ThrottleInput;
    float SteerInput;

    UPROPERTY()
    USkeletalMeshComponent* CarSkeletalMesh;

    UPROPERTY(EditAnywhere, Category = "Physics")
    float MoveForce = 100000.f;

    UPROPERTY(EditAnywhere, Category = "Physics")
    float TurnTorque = 500000.f;

    UPROPERTY(EditAnywhere, Category = "Physics")
    float MaxSpeed = 3000.f;

    UPROPERTY(EditAnywhere, Category = "Physics")
    float SidewaysFrictionStrength = 800.f;

    UPROPERTY(EditAnywhere, Category = "Physics")
    float ForwardFrictionStrength = 300.f;

    UPROPERTY(EditAnywhere, Category = "Physics")
    float SteeringGripStrength = 25000.f;

    UPROPERTY(EditAnywhere, Category = "Physics")
    float MaxSteeringAngle = 0.4f;

    UPROPERTY(VisibleAnywhere, Category = "Visuals")
    float SteeringAngle;

    UPROPERTY(VisibleAnywhere, Category = "Visuals")
    float WheelSpinAngle;

    UPROPERTY(EditAnywhere, Category = "Visuals")
    float WheelSpinSpeed = 500.f;

    UCameraComponent* BehindCamera;

    UCameraComponent* InsideCamera;

    UCameraComponent* HoodCamera;

    //UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    //URacingCarMovementComponent* MovementComponent;

    bool bIsStopped = false;

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

    float SpringStrength = 8000.0f;
    float SpringDamping = 2000.0f;

    UPROPERTY()
    TArray<UStaticMeshComponent*> MeshesWithLiveryMaterial;

    void GetAllLiveryMeshes();
    void ChangeMeshMaterial(int Index);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
    UMaterialInterface* TargetMaterial;

    UMaterialInterface* CurrentMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
    TArray<UMaterialInterface*> LiveryMaterials;

    UPROPERTY(EditAnywhere, Category = "Location")
    AStartingSpot* StartingSpot;

    void UpdateFOV();

};
