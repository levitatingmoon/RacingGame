#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GhostFrame.h"
#include "EnhancedInputComponent.h"
#include "RacingCar.generated.h"


class APraktykiGameModeBase;
class URaceWidget;
class UEndRaceWidget;
class UStartRaceWidget;
class UMainMenuWidget;
class ARacingCarMovementComponent;
class AMyPlayerController;
class AStartingSpot;
class AGhostCar;
class UNiagaraComponent;
class UAudioComponent;


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

    UStaticMeshComponent* SteeringWheel;
    float SteeringWheelAngle = 0.0f;

    TArray<FGhostFrame> CurrentLapFrames;

    float RecordingInterval = 0.33f; //30 FPS
    float RecordingTimer = 0.0f;
    bool bRecordingGhost = false;

    float PreviousThrottleValue = 0.0f;
    FVector2D ThrottleInput;
    float SteerInput;

    bool bPreviousGravelLB = false;
    bool bPreviousGravelRB = false;

    float GripFactor = 1.0f;
    float TireMass = 1.0f;

    float SpringStrength = 8000.0f;
    float SpringDamping = 2000.0f;

    bool bWasOffTrack = false;

    bool bIsStopped = false;

    UPROPERTY()
    AGhostCar* LastGhost = nullptr;

    UPROPERTY()
    UCameraComponent* BehindCamera;

    UPROPERTY()
    UCameraComponent* InsideCamera;

    UPROPERTY()
    UCameraComponent* HoodCamera;

    UPROPERTY()
    UNiagaraComponent* ThrottleParticles;

    UPROPERTY()
    UNiagaraComponent* WheelLBParticles;

    UPROPERTY()
    UNiagaraComponent* WheelRBParticles;

    void UpdateFOV();

    void SuspensionWheelForce();
    void SteerForce();

    void Throttle(const FInputActionValue& Value);
    void Steer(const FInputActionValue& Value);

    void UpdateSteeringWheel(float DeltaTime);

    void UseBehindCamera();
    void UseInsideCamera();
    void UseHoodCamera();

    void GetSteeringWheel();

    UCameraComponent* FindCameraByName(FName CameraName);

    void GetAllLiveryMeshes();

public:
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    
    void StopCar();
    void StartCar();
    void PrepareForRace();
    void OnLapCompleted();

    void StartGhostRecording();
    void StopGhostRecording();
    TArray<FGhostFrame>& GetRecordedGhostFrames();

    void ChangeMeshMaterial(int Index);

    bool bThisLapPenalty = false;
    float Penalty;

    UPROPERTY()
    UAudioComponent* EngineSound;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnPenaltyUpdated OnPenaltyUpdated;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ghost")
    TSubclassOf<AGhostCar> GhostCarClass;

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
    TArray<UStaticMeshComponent*> MeshesWithLiveryMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
    UMaterialInterface* TargetMaterial;

    UPROPERTY()
    UMaterialInterface* CurrentMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
    TArray<UMaterialInterface*> LiveryMaterials;

    UPROPERTY(EditAnywhere, Category = "Location")
    AStartingSpot* StartingSpot;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    class UInputMappingContext* IMC_CarControls;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    class UInputAction* IA_Throttle;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    class UInputAction* IA_Steer;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    class UInputAction* IA_BehindCamera;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    class UInputAction* IA_InsideCamera;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    class UInputAction* IA_HoodCamera;

};
