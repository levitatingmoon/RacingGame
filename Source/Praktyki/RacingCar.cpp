#include "RacingCar.h"
#include "PraktykiGameModeBase.h"
#include "RacingCarMovementComponent.h"
#include "RaceWidget.h"

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

    GameMode = Cast<APraktykiGameModeBase>(GetWorld()->GetAuthGameMode());
    NumberOfSectors = GameMode->SectorNumber;

    CurrentSectorTimes.SetNum(NumberOfSectors);
    BestSectorTimes.SetNum(NumberOfSectors);
    bSectorsStarted.SetNum(NumberOfSectors);

    APlayerController* PC = Cast<APlayerController>(GetController());
    if (PC && RaceWidgetClass)
    {
        RaceWidget = CreateWidget<URaceWidget>(PC, RaceWidgetClass);
        if (RaceWidget)
        {
            RaceWidget->AddToViewport();
        }
    }

    RaceWidget->Laps->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), StartedLaps, GameMode->LapLimit)));

}

void ARacingCar::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (GameMode)
    {
        RaceWidget->Timer->SetText(FText::FromString(FormatTime(GameMode->QualiTime, false)));
        // Update widget text
    }

    if (!bIsStopped)
    {
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
    if (!bIsStopped)
    {
        UE_LOG(LogTemp, Warning, TEXT("Throttle input: %f"), Val);
        ThrottleInput.X = FMath::Clamp(Val, -1.f, 1.f);
    }

}

void ARacingCar::Steer(float Val)
{
    if (!bIsStopped)
    {
        UE_LOG(LogTemp, Warning, TEXT("Steer input: %f"), Val);
        SteerInput = FMath::Clamp(Val, -1.f, 1.f);
    }

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

void ARacingCar::StartCar()
{
    bIsStopped = false;
}

void ARacingCar::StoreCheckpointTime(int SectorNumber, float TimerTime)
{
    if (SectorNumber == 0)
    {
        //Start of Race
        if (PreviousSectorNumber == 0)
        {
            StartLapTime = TimerTime;
            SectorStartTime = TimerTime;
        }
        //End of Lap
        else if (PreviousSectorNumber == NumberOfSectors - 1)
        {
            PreviousLapTime = TimerTime - StartLapTime;
            StartLapTime = TimerTime;
            SectorStartTime = TimerTime;

            if (PreviousLapTime < BestLapTime || StartedLaps == 1)
            {
                BestLapTime = PreviousLapTime;
                RaceWidget->BestLap->SetText(FText::FromString(FormatTime(BestLapTime, true)));
            }

            LapTimes.Add(PreviousLapTime);

            StartedLaps += 1;
            RaceWidget->Laps->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), StartedLaps, GameMode->LapLimit)));

            PreviousSectorNumber = SectorNumber;
        }

    }
    else 
    {
        FString Msg = FString::Printf(TEXT("QUALI TIME: %.2f"), CurrentSectorTimes.Num());
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, Msg);

        CurrentSectorTimes[SectorNumber-1] = TimerTime - SectorStartTime;

        SectorStartTime = TimerTime;

        if (PreviousSectorNumber == SectorNumber - 1)
        {
            PreviousSectorNumber = SectorNumber;
        }
    }

}

FString ARacingCar::FormatTime(float TimeSeconds, bool bMilliseconds)
{
    int32 TotalMilliseconds = FMath::RoundToInt(TimeSeconds * 1000.0f);
    int32 Minutes = TotalMilliseconds / 60000;
    int32 Seconds = (TotalMilliseconds % 60000) / 1000;
    if (bMilliseconds) 
    {
        int32 Milliseconds = TotalMilliseconds % 1000;
        return FString::Printf(TEXT("%02d:%02d:%03d"), Minutes, Seconds, Milliseconds);
    }

    return FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
}
