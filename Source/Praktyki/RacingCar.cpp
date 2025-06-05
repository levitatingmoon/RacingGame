#include "RacingCar.h"
#include "PraktykiGameModeBase.h"
#include "RacingCarMovementComponent.h"
#include "RaceWidget.h"
#include "EndRaceWidget.h"
#include "StartRaceWidget.h"
#include "MainMenuWidget.h"
#include "SurfaceTypes.h"
#include "Misc/Paths.h"
#include "MyPlayerController.h"
#include "RacingGameInstance.h"
#include "StartingSpot.h"
#include "GhostCar.h"


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

    FVector CenterOfMassOffset = FVector(0.f, 0.f, -200.f);
    CarSkeletalMesh->SetCenterOfMass(CenterOfMassOffset);

    GameMode = Cast<APraktykiGameModeBase>(GetWorld()->GetAuthGameMode());
    NumberOfSectors = GameMode->SectorNumber;

    CurrentSectorTimes.SetNum(NumberOfSectors);
    BestSectorTimes.SetNum(NumberOfSectors);
    PreviousBestSectorTimes.SetNum(NumberOfSectors);

    for (int i = 0; i < NumberOfSectors; i++)
    {
        BestSectorTimes[i] = FLT_MAX;
        PreviousBestSectorTimes[i] = FLT_MAX;
        CurrentSectorTimes[i] = 0.0f;
    }

    GetAllLiveryMeshes();
    CurrentMaterial = TargetMaterial;

    UGameInstance* GI = GetWorld()->GetGameInstance();
    URacingGameInstance* GameInstance = Cast<URacingGameInstance>(GI);

    ChangeMeshMaterial(GameInstance->MaterialIndex);
}

void ARacingCar::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    UpdateFOV();

    if (!bIsStopped)
    {

        if (bRecordingGhost)
        {
            RecordingTimer += DeltaTime;
            while (RecordingTimer >= RecordingInterval)
            {
                RecordingTimer -= RecordingInterval;
                CurrentLapFrames.Add(FGhostFrame(GetActorLocation(), GetActorRotation()));
            }
        }

        SuspensionWheelForce();
        
        FVector Velocity = CarSkeletalMesh->GetComponentVelocity();
        FVector ForwardVector = CarSkeletalMesh->GetForwardVector();
        FVector RightVector = CarSkeletalMesh->GetRightVector();

        float SidewaysSpeed = FVector::DotProduct(Velocity, RightVector);
        FVector SidewaysFrictionForce = -RightVector * SidewaysSpeed * SidewaysFrictionStrength;
        CarSkeletalMesh->AddForce(SidewaysFrictionForce);

        //Slowing down when no input
        float ForwardSpeed = FVector::DotProduct(Velocity, ForwardVector);
        if (ThrottleInput.IsNearlyZero())
        {
            FVector ForwardResistance = -ForwardVector * ForwardSpeed * ForwardFrictionStrength;
            CarSkeletalMesh->AddForce(ForwardResistance);
        }
        
        //adding force when input
        if (!ThrottleInput.IsNearlyZero())
        {
            float Speed = CarSkeletalMesh->GetComponentVelocity().Size();
            float SpeedRatio = FMath::Clamp(Speed / MaxSpeed, 0.f, 1.f);
            float ForceScale = 1.f - SpeedRatio;

            FVector Force = CarSkeletalMesh->GetForwardVector() * ThrottleInput.X * MoveForce * ForceScale;
            CarSkeletalMesh->AddForce(Force);

        }

        
        if (FMath::Abs(SteerInput) > 0.01f)
        {
            FVector Torque = FVector(0.f, 0.f, 1.f) * SteerInput * TurnTorque;
            CarSkeletalMesh->AddTorqueInRadians(Torque);

            //SteerForce();
        }

        //SteerForce();
        
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
        //UE_LOG(LogTemp, Warning, TEXT("Throttle input: %f"), Val);
        ThrottleInput.X = FMath::Clamp(Val, -1.f, 1.f);
    }

}

void ARacingCar::Steer(float Val)
{
    if (!bIsStopped)
    {
        //UE_LOG(LogTemp, Warning, TEXT("Steer input: %f"), Val);
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

    UE_LOG(LogTemp, Warning, TEXT("STOPPED"));
}

void ARacingCar::StartCar()
{
    UE_LOG(LogTemp, Warning, TEXT("START"));
    bIsStopped = false;
}

void ARacingCar::PrepareForRace()
{
    StartedLaps = 1;
    SectorStartTime = 0;
    PreviousSectorNumber = 0;
    PreviousLapTime = 0;
    StartLapTime = 0;
    Penalty = 0.0f;

    SetActorLocationAndRotation(StartingSpot->GetActorLocation(),StartingSpot->GetActorRotation(),false,nullptr,ETeleportType::TeleportPhysics);

}

void ARacingCar::OnLapCompleted()
{
    TArray<FGhostFrame> FinishedLapFrames = CurrentLapFrames;
    UE_LOG(LogTemp, Warning, TEXT("Recorded %d frames, total time: %.2f"), CurrentLapFrames.Num(), RecordingInterval * CurrentLapFrames.Num());

    CurrentLapFrames.Empty();
    RecordingTimer = 0.0f;

    if (LastGhost)
    {
        LastGhost->Destroy();
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    FVector SpawnLoc = FinishedLapFrames.Num() > 0 ? FinishedLapFrames[0].Location : GetActorLocation();
    FRotator SpawnRot = FinishedLapFrames.Num() > 0 ? FinishedLapFrames[0].Rotation : GetActorRotation();
    
    if (!bIsGhost)
    {
        AGhostCar* NewGhost = GetWorld()->SpawnActor<AGhostCar>(GhostCarClass, SpawnLoc, SpawnRot, SpawnParams);

        if (NewGhost)
        {
            NewGhost->AutoPossessPlayer = EAutoReceiveInput::Disabled;
            NewGhost->AutoPossessAI = EAutoPossessAI::Disabled;
            NewGhost->CarSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            NewGhost->LoadGhostData(FinishedLapFrames);
            NewGhost->StartGhostPlayback();
            LastGhost = NewGhost;
        }
    }
    

}

void ARacingCar::GetAllLiveryMeshes()
{
    TArray<USceneComponent*> AllChildrenComponents;
    CarSkeletalMesh->GetChildrenComponents(true, AllChildrenComponents);

    for (USceneComponent* ChildComp : AllChildrenComponents)
    {
        if (UStaticMeshComponent* StaticMeshComp = Cast<UStaticMeshComponent>(ChildComp))
        {
            if (StaticMeshComp)
            {
                int32 NumMaterials = StaticMeshComp->GetNumMaterials();
                for (int32 MatIndex = 0; MatIndex < NumMaterials; ++MatIndex)
                {
                    if (StaticMeshComp->GetMaterial(MatIndex) == TargetMaterial)
                    {
                        MeshesWithLiveryMaterial.Add(StaticMeshComp);
                        break;
                    }
                }
            }
        }
    }
}

void ARacingCar::ChangeMeshMaterial(int Index)
{
    for (UStaticMeshComponent* StaticMesh : MeshesWithLiveryMaterial)
    {

        int32 NumMaterials = StaticMesh->GetNumMaterials();
        for (int32 MatIndex = 0; MatIndex < NumMaterials; ++MatIndex)
        {

            if (StaticMesh->GetMaterial(MatIndex) == CurrentMaterial)
            {

                StaticMesh->SetMaterial(MatIndex, LiveryMaterials[Index]);
            }
        }
    }
    CurrentMaterial = LiveryMaterials[Index];
}

void ARacingCar::UpdateFOV()
{

    float Speed = GetVelocity().Size();
    float SpeedFactor = FMath::Clamp(Speed / MaxSpeed, 0.0f, 1.0f);

    float NewFOV = FMath::Lerp(90.0f, 120.0f, SpeedFactor);

    if (BehindCamera->IsActive())
    {
        BehindCamera->SetFieldOfView(NewFOV);
    }
    else if (InsideCamera->IsActive())
    {
        InsideCamera->SetFieldOfView(NewFOV);
    }
    else if (HoodCamera->IsActive())
    {
        HoodCamera->SetFieldOfView(NewFOV);
    }
    
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

            DrawDebugLine(GetWorld(), Start, Start + (springDir * force)/10000.0f, FColor::Blue, false, 2.0f, 0, 2.0f);
        }

    }

    if (WheelsOffTrack == 4)
    {
        bIsOffTrack = true;
    }

    if (bIsOffTrack && !bWasOffTrack)
    {
        Penalty += 5;
        bThisLapPenalty = true;
        OnPenaltyUpdated.Broadcast(Penalty);
    }

    bWasOffTrack = bIsOffTrack;
}

//TESTING
void ARacingCar::SteerForce()
{
    for (const FName& Bone : WheelBones)
    {
        //UE_LOG(LogTemp, Warning, TEXT("SteerInput: %f"), SteerInput);

        FRotator BoneRotation = CarSkeletalMesh->GetBoneQuaternion(Bone).Rotator();
        FVector SteeringDir = BoneRotation.RotateVector(FVector::RightVector);

        //UE_LOG(LogTemp, Warning, TEXT("Wheel %s SteeringDir: %s"), *Bone.ToString(), *SteeringDir.ToString());

        FVector WheelVelocity = CarSkeletalMesh->GetPhysicsLinearVelocityAtPoint(CarSkeletalMesh->GetBoneLocation(Bone));


        if (Bone == WheelBones[0] || Bone == WheelBones[1])
        {

            FVector ForwardDir = BoneRotation.RotateVector(FVector::ForwardVector);
            float ForwardSpeed = FVector::DotProduct(WheelVelocity, ForwardDir);

            float SteeringStrength = FMath::Clamp(ForwardSpeed * 2.0f, 10.f, 50.f);

            //FVector LateralSteeringForce = SteeringDir * SteerInput * SteeringStrength;

            FVector LateralSteeringForce = SteeringDir * SteerInput * ForwardSpeed * SteeringStrength; //SteeringStrength;

            UE_LOG(LogTemp, Warning, TEXT("FORWARD: %f"), ForwardSpeed);
            if (FMath::Abs(SteerInput) > 0.01f)
            {
                CarSkeletalMesh->AddForceAtLocation(LateralSteeringForce, CarSkeletalMesh->GetBoneLocation(Bone));
            }
            //UE_LOG(LogTemp, Warning, TEXT("FORCE: %f %f %f"), LateralSteeringForce.X, LateralSteeringForce.Y, LateralSteeringForce.Z);

            DrawDebugLine(GetWorld(), CarSkeletalMesh->GetBoneLocation(Bone), CarSkeletalMesh->GetBoneLocation(Bone) + LateralSteeringForce * 0.01f, FColor::Green, false, 0.1f, 0, 2.0f);
        
        }
        
        float SteeringVelocity = FVector::DotProduct(SteeringDir, WheelVelocity);

        float DesiredVelChange = -SteeringVelocity * GripFactor; //0-1 friction

        float DesiredAccel = DesiredVelChange / GetWorld()->GetDeltaSeconds();

        FVector GripForce = SteeringDir * DesiredAccel * TireMass;

        //UE_LOG(LogTemp, Warning, TEXT("GRIP: %f %f %f"), GripForce.X, GripForce.Y, GripForce.Z);
        if (FMath::Abs(SteerInput) > 0.01f)
        {
            CarSkeletalMesh->AddForceAtLocation(GripForce, CarSkeletalMesh->GetBoneLocation(Bone));
        }
        DrawDebugLine(GetWorld(), CarSkeletalMesh->GetBoneLocation(Bone), CarSkeletalMesh->GetBoneLocation(Bone) + GripForce * 0.01f, FColor::Red, false, 0.1f, 0, 2.0f);

        
    }
}

void ARacingCar::StartGhostRecording()
{
    bRecordingGhost = true;
    RecordingTimer = 0.0f;
    CurrentLapFrames.Empty();
}

void ARacingCar::StopGhostRecording()
{
    bRecordingGhost = false;
}

TArray<FGhostFrame>& ARacingCar::GetRecordedGhostFrames()
{
    return CurrentLapFrames;
}
