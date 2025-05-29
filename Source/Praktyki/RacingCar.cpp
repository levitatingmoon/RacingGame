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

    if (!ThrottleInput.IsNearlyZero())
    {
        UE_LOG(LogTemp, Warning, TEXT("Applying force: %f"), ThrottleInput.X * MoveForce);
        FVector Force = CarSkeletalMesh->GetForwardVector() * ThrottleInput.X * MoveForce;
        CarSkeletalMesh->AddForce(Force);
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
