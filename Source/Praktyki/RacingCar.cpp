#include "RacingCar.h"
#include "PraktykiGameModeBase.h"
#include "RacingCarMovementComponent.h"
#include "RaceWidget.h"
#include "EndRaceWidget.h"
#include "StartRaceWidget.h"
#include "SurfaceTypes.h"

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

    APlayerController* PC = Cast<APlayerController>(GetController());
    if (PC && RaceWidgetClass)
    {
        RaceWidget = CreateWidget<URaceWidget>(PC, RaceWidgetClass);
        if (RaceWidget)
        {
            RaceWidget->AddToViewport();
            RaceWidget->Laps->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), StartedLaps, GameMode->LapLimit)));
        }
    }


    if (PC && StartRaceWidgetClass)
    {
        FString Msg = FString::Printf(TEXT("START RACE CLASS"));
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, Msg);
        StartRaceWidget = CreateWidget<UStartRaceWidget>(PC, StartRaceWidgetClass);
    }

}

void ARacingCar::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (GameMode)
    {
        RaceWidget->Timer->SetText(FText::FromString(FormatTime(GameMode->QualiTime, false)));
        if (bStartedFirstLap)
        {
            RaceWidget->SectorTimer->SetText(FText::FromString(FormatTime(GameMode->QualiTime - SectorStartTime, false)));
        }

    }

    if (!bIsStopped)
    {
        SuspensionWheelForce();

        
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

void ARacingCar::PrepareForRace()
{
    StartedLaps = 1;
    SectorStartTime = 0;
    PreviousSectorNumber = 0;
    PreviousLapTime = 0;

    SetActorLocation(FVector(-2000.f, 0.f, 200.f), false, nullptr, ETeleportType::TeleportPhysics);
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
            bStartedFirstLap = true;
        }
        //End of Lap
        else if (PreviousSectorNumber == NumberOfSectors - 1)
        {
            PreviousLapTime = TimerTime - StartLapTime;
            StartLapTime = TimerTime;
            SectorStartTime = TimerTime;

            if (GameMode->bIsRace)
            {
                if (PreviousLapTime < BestRaceLap || StartedLaps == 1)
                {
                    BestRaceLap = PreviousLapTime;
                    RaceWidget->BestLap->SetText(FText::FromString(FormatTime(BestRaceLap, true)));
                }

                //LapTimes.Add(PreviousLapTime);

                StartedLaps += 1;
                RaceWidget->Laps->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), StartedLaps, GameMode->LapLimit)));
            }
            else if (GameMode->bIsQuali)
            {
                if(PreviousLapTime < BestQualiLap || StartedLaps == 1)
                {
                    BestQualiLap = PreviousLapTime;
                    RaceWidget->BestLap->SetText(FText::FromString(FormatTime(BestQualiLap, true)));
                }

                //LapTimes.Add(PreviousLapTime);

                StartedLaps += 1;
                RaceWidget->Laps->SetText(FText::FromString(FString::Printf(TEXT("%d"), StartedLaps)));
            }

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

    if (StartedLaps > GameMode->LapLimit && !bRaceEnded && GameMode->bIsRace)
    {
        bRaceEnded = true;
        StopCar();
        GetEndRaceStatistics();
    }

}

void ARacingCar::GetEndRaceStatistics()
{
    RaceWidget->RemoveFromParent();

    APlayerController* PC = Cast<APlayerController>(GetController());
    if (PC && EndRaceWidgetClass)
    {
        EndRaceWidget = CreateWidget<UEndRaceWidget>(PC, EndRaceWidgetClass);
        if (EndRaceWidget)
        {
            EndRaceWidget->AddToViewport();
            PC->bShowMouseCursor = true;

            EndRaceWidget->BestTime->SetText(FText::FromString(FormatTime(BestQualiLap, true)));
            EndRaceWidget->FastestLap->SetText(FText::FromString(FormatTime(BestRaceLap, true)));
            EndRaceWidget->Penalties->SetText(FText::FromString(FormatTime(Penalty, true)));
            EndRaceWidget->RaceTime->SetText(FText::FromString(FormatTime(GameMode->QualiTime, true)));
        }

    }
  
}

void ARacingCar::StartRaceCountdown()
{
    RaceWidget->SetVisibility(ESlateVisibility::Hidden);

    APlayerController* PC = Cast<APlayerController>(GetController());
    if (PC && StartRaceWidgetClass)
    {
        StartRaceWidget = CreateWidget<UStartRaceWidget>(PC, StartRaceWidgetClass);
        if (StartRaceWidget)
        {
            StartRaceWidget->AddToViewport();
        }
    }
}

void ARacingCar::LightOn(int LightIndex)
{
    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("LIGHT ON"));
    StartRaceWidget->SetLightColour(LightIndex, FLinearColor::Red);
}

void ARacingCar::LightsOut()
{
    //StartRaceWidget->SetLightColour(0, FLinearColor::Black);
    //StartRaceWidget->SetLightColour(1, FLinearColor::Black);
    //StartRaceWidget->SetLightColour(2, FLinearColor::Black);
    //StartRaceWidget->SetLightColour(3, FLinearColor::Black);
    //StartRaceWidget->SetLightColour(4, FLinearColor::Black);

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

void ARacingCar::SuspensionWheelForce()
{
    int WheelsOffTrack = 0;
    bool bIsOffTrack = false;
    for (const FName& Bone : WheelBones)
    {

        FVector BoneLocation = CarSkeletalMesh->GetBoneLocation(Bone);
        FRotator BoneRotation = CarSkeletalMesh->GetBoneQuaternion(Bone).Rotator();
        FVector BoneRightVector = BoneRotation.RotateVector(FVector::RightVector);

        float SideMultiplier = 1.0f;

        if (Bone == WheelBones[1] || Bone == WheelBones[3])
        {
            SideMultiplier = -1.0f;
        }

        FVector SideOffset = BoneRightVector * 20.f * SideMultiplier;
        FVector Start = BoneLocation + SideOffset;
        FVector End = Start - FVector(0, 0, 100);

        FHitResult Hit;
        FCollisionQueryParams TraceParams(FName(TEXT("SuspensionTrace")), true, this);
        TraceParams.AddIgnoredActor(this);
        TraceParams.bReturnPhysicalMaterial = true;
        bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, TraceParams);

        if (bHit)
        {

            UPhysicalMaterial* PhysMat = Hit.PhysMaterial.Get();
            if (PhysMat && PhysMat->SurfaceType != SURFACE_Asphalt)
            {
                WheelsOffTrack += 1;
            }

            FVector springDir = FVector::UpVector;
            FVector tireWorldVel = CarSkeletalMesh->GetPhysicsLinearVelocityAtPoint(Start);

            float offset = 50.0f - Hit.Distance;

            float vel = FVector::DotProduct(springDir, tireWorldVel);
            float force = (offset * SpringStrength) - (vel * SpringDamping);

            CarSkeletalMesh->AddForceAtLocation(springDir * force, Start);

            //SteerForce(Bone);

            DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.0f, 0, 2.0f);
        }

    }

    if (WheelsOffTrack == 4)
    {
        bIsOffTrack = true;
    }

    if (bIsOffTrack && !bWasOffTrack)
    {
        Penalty += 5;
        FString Msg = FString::Printf(TEXT("BONE: %.2f"), Penalty);
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, Msg);
    }

    bWasOffTrack = bIsOffTrack;
}

//TESTING
void ARacingCar::SteerForce(FName Bone)
{

    FVector SteeringDir = CarSkeletalMesh->GetSocketTransform(Bone, ERelativeTransformSpace::RTS_Component).GetUnitAxis(EAxis::Y);


    FVector TireWorldVelocity = CarSkeletalMesh->GetPhysicsLinearVelocityAtPoint(CarSkeletalMesh->GetBoneLocation(Bone));

    float LateralVelocity = FVector::DotProduct(SteeringDir, TireWorldVelocity);

    float DesiredVelChange = -LateralVelocity * GripFactor;

    float DesiredAccel = DesiredVelChange / GetWorld()->GetDeltaSeconds();

    float Damping = 0.7f;
    DesiredAccel = FMath::Lerp(0.0f, DesiredAccel, Damping);

    FVector LateralForce = SteeringDir * DesiredAccel * TireMass;

    float MaxLateralForce = 5000.0f;
    LateralForce = LateralForce.GetClampedToMaxSize(MaxLateralForce);

    CarSkeletalMesh->AddForceAtLocation(LateralForce, CarSkeletalMesh->GetBoneLocation(Bone));
}
