#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "RacingCar.generated.h"

class APraktykiGameModeBase;
class URaceWidget;
class ARacingCarMovementComponent;

UCLASS()
class PRAKTYKI_API ARacingCar : public APawn
{
    GENERATED_BODY()

public:
    ARacingCar();

protected:
    virtual void BeginPlay() override;

private:

public:
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    void Throttle(float Val);
    void Steer(float Val);

    void UseBehindCamera();
    void UseInsideCamera();
    void UseHoodCamera();

    UCameraComponent* FindCameraByName(FName CameraName);

    void StopCar();

    void StartCar();

    void StoreCheckpointTime(int SectorNumber, float QualiTime);

    FString FormatTime(float TimeSeconds, bool bMilliseconds);

    TArray<float> CurrentSectorTimes;
    TArray<float> BestSectorTimes;
    TArray<bool> bSectorsDone;
    TArray<float> LapTimes;

    int NumberOfSectors = 0;

    float PreviousLapTime;
    float BestLapTime;
    float StartLapTime;
    float SectorStartTime;

    int StartedLaps = 0;

    bool bPassedAllSectors = false;

    FVector2D ThrottleInput;
    float SteerInput;

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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visuals")
    UStaticMesh* Wheel_FL;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visuals")
    UStaticMesh* Wheel_FR;

    bool bIsStopped = false;

    URaceWidget* RaceWidget;

    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<URaceWidget> RaceWidgetClass;

    APraktykiGameModeBase* GameMode;

};
