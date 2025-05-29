#include "RacingCar.h"

ARacingCar::ARacingCar()
{
    PrimaryActorTick.bCanEverTick = true;

    //MovementComponent = CreateDefaultSubobject<URacingCarMovementComponent>(TEXT("MovementComponent"));
    //MovementComponent->SetUpdatedComponent(RootComponent);
}

void ARacingCar::BeginPlay()
{
    Super::BeginPlay();

    CarSkeletalMesh = Cast<USkeletalMeshComponent>(GetRootComponent());
    if (!CarSkeletalMesh)
    {
        UE_LOG(LogTemp, Error, TEXT("CarSkeletalMesh is null!"));
    }

    FVector CenterOfMassOffset = FVector(0.f, 0.f, -200.f);
    CarSkeletalMesh->SetCenterOfMass(CenterOfMassOffset);
}

void ARacingCar::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    FVector Velocity = CarSkeletalMesh->GetComponentVelocity();
    FVector ForwardVector = CarSkeletalMesh->GetForwardVector();
    FVector RightVector = CarSkeletalMesh->GetRightVector();

    float SidewaysSpeed = FVector::DotProduct(Velocity, RightVector);
    FVector SidewaysFrictionForce = -RightVector * SidewaysSpeed * SidewaysFrictionStrength;
    CarSkeletalMesh->AddForce(SidewaysFrictionForce);

    float ForwardSpeed = FVector::DotProduct(Velocity, ForwardVector);
    if (ThrottleInput.IsNearlyZero())
    {
        FVector ForwardResistance = -ForwardVector * ForwardSpeed * ForwardFrictionStrength;
        CarSkeletalMesh->AddForce(ForwardResistance);
    }

    if (!ThrottleInput.IsNearlyZero())
    {
        float Speed = CarSkeletalMesh->GetComponentVelocity().Size();
        float SpeedRatio = FMath::Clamp(Speed / MaxSpeed, 0.f, 1.f);
        float ForceScale = 1.f - SpeedRatio;

        FVector Force = CarSkeletalMesh->GetForwardVector() * ThrottleInput.X * MoveForce * ForceScale;
        CarSkeletalMesh->AddForce(Force);

        UE_LOG(LogTemp, Warning, TEXT("Speed: %.1f, Force scale: %.2f"), Speed, ForceScale);
    }

    if (FMath::Abs(SteerInput) > 0.1f)
    {
        FVector Torque = FVector(0.f, 0.f, 1.f) * SteerInput * TurnTorque;
        CarSkeletalMesh->AddTorqueInRadians(Torque);
    }
}

void ARacingCar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    PlayerInputComponent->BindAxis("Throttle", this, &ARacingCar::Throttle);
    PlayerInputComponent->BindAxis("Steer", this, &ARacingCar::Steer);
}

void ARacingCar::Throttle(float Val)
{
    UE_LOG(LogTemp, Warning, TEXT("Throttle input: %f"), Val);
    ThrottleInput.X = FMath::Clamp(Val, -1.f, 1.f);
}

void ARacingCar::Steer(float Val)
{
    UE_LOG(LogTemp, Warning, TEXT("Steer input: %f"), Val);
    SteerInput = FMath::Clamp(Val, -1.f, 1.f);
}
