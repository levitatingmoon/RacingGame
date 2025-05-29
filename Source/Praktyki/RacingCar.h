#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "RacingCarMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "RacingCar.generated.h"

UCLASS()
class PRAKTYKI_API ARacingCar : public APawn
{
    GENERATED_BODY()

public:
    ARacingCar();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    void Throttle(float Val);
    void Steer(float Val);

    FVector2D ThrottleInput;
    float SteerInput;

    // Movement parameters
    USkeletalMeshComponent* CarSkeletalMesh;

    UPROPERTY(EditAnywhere, Category = "Physics")
    float MoveForce = 100000.f;

    UPROPERTY(EditAnywhere, Category = "Physics")
    float TurnTorque = 500000.f;

    // Visual rotation tracking
    UPROPERTY(VisibleAnywhere, Category = "Visuals")
    float SteeringAngle;

    UPROPERTY(VisibleAnywhere, Category = "Visuals")
    float WheelSpinAngle;

    UPROPERTY(EditAnywhere, Category = "Visuals")
    float MaxSteeringAngle = 30.f;

    UPROPERTY(EditAnywhere, Category = "Visuals")
    float WheelSpinSpeed = 500.f;

    //UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    //URacingCarMovementComponent* MovementComponent;



};
