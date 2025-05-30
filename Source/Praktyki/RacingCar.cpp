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

    BehindCamera = FindCameraByName(TEXT("BehindCamera"));
    InsideCamera = FindCameraByName(TEXT("InsideCamera"));
    HoodCamera = FindCameraByName(TEXT("HoodCamera"));

    UseBehindCamera();

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

    if (FMath::Abs(SteerInput) > 0.01f)
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

    PlayerInputComponent->BindAction("BehindCamera", IE_Pressed, this, &ARacingCar::UseBehindCamera);
    PlayerInputComponent->BindAction("InsideCamera", IE_Pressed, this, &ARacingCar::UseInsideCamera);
    PlayerInputComponent->BindAction("HoodCamera", IE_Pressed, this, &ARacingCar::UseHoodCamera);
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

void ARacingCar::UseBehindCamera()
{
    BehindCamera->SetActive(true);
    InsideCamera->SetActive(false);
    HoodCamera->SetActive(false);
}

void ARacingCar::UseInsideCamera()
{
    BehindCamera->SetActive(false);
    InsideCamera->SetActive(true);
    HoodCamera->SetActive(false);
}

void ARacingCar::UseHoodCamera()
{
    BehindCamera->SetActive(false);
    InsideCamera->SetActive(false);
    HoodCamera->SetActive(true);
}

UCameraComponent* ARacingCar::FindCameraByName(FName CameraName)
{
    TArray<UCameraComponent*> Cameras;
    GetComponents<UCameraComponent>(Cameras);

    for (UCameraComponent* Camera : Cameras)
    {
        if (Camera->GetName() == CameraName.ToString())
        {
            return Camera;
        }
    }

    return nullptr;
}

void ARacingCar::StopCar()
{
    bIsStopped = true;
    CarSkeletalMesh->SetAllPhysicsLinearVelocity(FVector::ZeroVector);
    CarSkeletalMesh->SetAllPhysicsAngularVelocityInDegrees(FVector::ZeroVector);

    UE_LOG(LogTemp, Warning, TEXT("StoppedCar"));
}

void ARacingCar::StoreCheckpointTime(int SectorNumber, float QualiTime)
{
    if (SectorNumber == 0)
    {
        PreviousLapTime = QualiTime - StartLapTime;
        StartLapTime = QualiTime;
        SectorStartTime = QualiTime;

        if (PreviousLapTime < BestLapTime) BestLapTime = PreviousLapTime;

    }
    else 
    {
        CurrentSectorTimes[SectorNumber] = QualiTime - SectorStartTime;
        SectorStartTime = QualiTime;

    }
}
