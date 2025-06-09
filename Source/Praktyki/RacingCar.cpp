#include "RacingCar.h"
#include "PraktykiGameModeBase.h"
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
#include "NiagaraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/AudioComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"


ARacingCar::ARacingCar()
{
    PrimaryActorTick.bCanEverTick = true;

}

void ARacingCar::BeginPlay()
{
    Super::BeginPlay();

    CarSkeletalMesh = Cast<USkeletalMeshComponent>(GetRootComponent());

    UGameInstance* GI = GetWorld()->GetGameInstance();
    URacingGameInstance* GameInstance = Cast<URacingGameInstance>(GI);

    BehindCamera = FindCameraByName(TEXT("CameraBehind"));
    InsideCamera = FindCameraByName(TEXT("CameraInside"));
    HoodCamera = FindCameraByName(TEXT("CameraHood"));

    ThrottleParticles = Cast<UNiagaraComponent>(GetDefaultSubobjectByName(TEXT("ExhaustFlame")));
    WheelLBParticles = Cast<UNiagaraComponent>(GetDefaultSubobjectByName(TEXT("WheelLeftBackEmitter")));
    WheelRBParticles = Cast<UNiagaraComponent>(GetDefaultSubobjectByName(TEXT("WheelRightBackEmitter")));

    UseBehindCamera();

    FVector CenterOfMassOffset = FVector(0.f, 0.f, -200.f);
    CarSkeletalMesh->SetCenterOfMass(CenterOfMassOffset);

    GetAllLiveryMeshes();
    CurrentMaterial = TargetMaterial;
    ChangeMeshMaterial(GameInstance->MaterialIndex);

    GetSteeringWheel();

    TArray<UActorComponent*> AudioComponents;
    GetComponents(UAudioComponent::StaticClass(), AudioComponents);

    for (UActorComponent* Audio : AudioComponents)
    {
        UAudioComponent* AudioComponent = Cast<UAudioComponent>(Audio);
        if (AudioComponent && AudioComponent->GetName() == TEXT("SoundEngine"))
        {
            EngineSound = AudioComponent;
            break;
        }
    }

    if (AMyPlayerController* PC = Cast<AMyPlayerController>(GetController()))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
            ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(IMC_CarControls, 0);
        }
    }
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

        UpdateSteeringWheel(DeltaTime);
        SuspensionWheelForce();
        
        
        FVector Velocity = CarSkeletalMesh->GetComponentVelocity();
        FVector ForwardVector = CarSkeletalMesh->GetForwardVector();
        FVector RightVector = CarSkeletalMesh->GetRightVector();

        float SidewaysSpeed = FVector::DotProduct(Velocity, RightVector);
        FVector SidewaysFrictionForce = -RightVector * SidewaysSpeed * SidewaysFrictionStrength * SurfaceFriction;
        CarSkeletalMesh->AddForce(SidewaysFrictionForce);

        //Slowing down when no input
        float ForwardSpeed = FVector::DotProduct(Velocity, ForwardVector);
        if (ThrottleInput.IsNearlyZero())
        {
            FVector ForwardResistance = -ForwardVector * ForwardSpeed * ForwardFrictionStrength;
            CarSkeletalMesh->AddForce(ForwardResistance);
        }
        
        //Adding force when input
        if (!ThrottleInput.IsNearlyZero())
        {
            float Speed = CarSkeletalMesh->GetComponentVelocity().Size();
            float SpeedRatio = FMath::Clamp(Speed / MaxSpeed, 0.f, 1.f);
            float ForceScale = 1.f - SpeedRatio;

            float InputX = ThrottleInput.X;
            FString DebugForce = FString::Printf(TEXT("InputX: %.3f"), InputX);
            GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Green, DebugForce);

            //Acceleration || Reverse
            if (InputX > 0.0f || (InputX < 0.0f && ForwardSpeed <= 0.0f))
            {
                float SurfaceThrottleFactor = FMath::Clamp(FMath::Pow(1.f / SurfaceFriction, 1.5f), 0.05f, 1.f);
                FVector Force = ForwardVector * InputX * MoveForce * ForceScale * SurfaceThrottleFactor;
                DebugForce = FString::Printf(TEXT("Throttle Force: %.1f"), Force.Size());
                GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Green, DebugForce);
                CarSkeletalMesh->AddForce(Force);
            }
            //Braking
            else if(InputX < 0.0f && ForwardSpeed > 0.0f)
            {
                float BrakingFactor = FMath::Clamp(SurfaceFriction * 1.5f, 1.f, 5.f);
                FVector BrakingForce = ForwardVector * InputX * BrakeForce * ForceScale * BrakingFactor;

                FString DebugBrake = FString::Printf(TEXT("Braking Force: %.1f"), BrakingForce.Size());
                GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red, DebugBrake);
                CarSkeletalMesh->AddForce(BrakingForce);
            }

        }

        FString FrictionText = FString::Printf(TEXT("Surface Friction: %.2f"), SurfaceFriction);
        GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Yellow, FrictionText);

        
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

    if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) 
    {
        EnhancedInputComponent->BindAction(IA_Throttle, ETriggerEvent::Triggered, this, &ARacingCar::Throttle);
        EnhancedInputComponent->BindAction(IA_Throttle, ETriggerEvent::Completed, this, &ARacingCar::ThrottleCompleted);
        EnhancedInputComponent->BindAction(IA_Steer, ETriggerEvent::Triggered, this, &ARacingCar::Steer);
        EnhancedInputComponent->BindAction(IA_Steer, ETriggerEvent::Completed, this, &ARacingCar::SteerCompleted);

        EnhancedInputComponent->BindAction(IA_BehindCamera, ETriggerEvent::Triggered, this, &ARacingCar::UseBehindCamera);
        EnhancedInputComponent->BindAction(IA_InsideCamera, ETriggerEvent::Triggered, this, &ARacingCar::UseInsideCamera);
        EnhancedInputComponent->BindAction(IA_HoodCamera, ETriggerEvent::Triggered, this, &ARacingCar::UseHoodCamera);
    }
}

void ARacingCar::Throttle(const FInputActionValue& Value)
{

    if (!bIsStopped)
    {
        float Val = Value.Get<float>();
        ThrottleInput.X = FMath::Clamp(Val, -1.f, 1.f);

        if (EngineSound)
        {
            if (ThrottleInput.X > 0.1f)
            {
                if (!EngineSound->IsPlaying())
                {
                    EngineSound->Play();
                }
                EngineSound->SetVolumeMultiplier(1.0f);
            }
            else if (ThrottleInput.X < -0.1f)
            {
                if (!EngineSound->IsPlaying())
                {
                    EngineSound->Play();
                }
                EngineSound->SetVolumeMultiplier(0.3f);
            }
            else
            {
                EngineSound->Stop();
            }
        }

        if (PreviousThrottleValue <= 0.0f && Val > 0.1f && ThrottleParticles)
        {
            ThrottleParticles->Activate(true);
        }

        PreviousThrottleValue = Val;
    }
}

void ARacingCar::ThrottleCompleted(const FInputActionValue& Value)
{
    ThrottleInput.X = 0.f;
}

void ARacingCar::Steer(const FInputActionValue& Value)
{
    if (!bIsStopped)
    {
        float Val = Value.Get<float>();
        SteerInput = FMath::Clamp(Val, -1.f, 1.f);

        float MaxSteerAngle = 45.0f;

        if (SteeringWheel)
        {
            float WheelAngle = SteerInput * MaxSteerAngle;

            float InterpSpeed = 1.0f;
            SteeringWheelAngle = FMath::FInterpTo(SteeringWheelAngle, WheelAngle, GetWorld()->GetDeltaSeconds(), InterpSpeed);
            FRotator NewRotation = FRotator(0.f, 0.f, SteeringWheelAngle);

            SteeringWheel->SetRelativeRotation(NewRotation);
        }

    }

}

void ARacingCar::SteerCompleted(const FInputActionValue& Value)
{
    SteerInput = 0.f;
}

void ARacingCar::UpdateSteeringWheel(float DeltaTime)
{
    if (SteeringWheel)
    {
        float MaxSteerAngle = 45.0f;
        float TargetAngle = SteerInput * MaxSteerAngle;

        float InterpSpeed = 5.0f;
        SteeringWheelAngle = FMath::FInterpTo(SteeringWheelAngle, TargetAngle, DeltaTime, InterpSpeed);

        FRotator NewRotation(0.f, 0.f, SteeringWheelAngle);
        SteeringWheel->SetRelativeRotation(NewRotation);
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

void ARacingCar::GetSteeringWheel()
{
    TArray<USceneComponent*> AllChildrenComponents;
    CarSkeletalMesh->GetChildrenComponents(true, AllChildrenComponents);

    for (USceneComponent* Child : AllChildrenComponents)
    {
        if (UStaticMeshComponent* StaticMesh = Cast<UStaticMeshComponent>(Child))
        {
            if (StaticMesh && StaticMesh->GetName() == TEXT("Steering_Wheel"))
            {
                SteeringWheel = StaticMesh;
                break;
            }
        }
    }
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
}

void ARacingCar::StartCar()
{
    bIsStopped = false;
}

void ARacingCar::PrepareForRace()
{
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

    FVector SpawnLoc;
    FRotator SpawnRot;

    if (FinishedLapFrames.Num() > 0)
    {
        SpawnLoc = FinishedLapFrames[0].Location;
        SpawnRot = FinishedLapFrames[0].Rotation;
    }
    else
    {
        SpawnLoc = GetActorLocation();
        SpawnRot = GetActorRotation();
    }
    
    UE_LOG(LogTemp, Warning, TEXT("Ghost Spawn: First frame at %s, Num frames: %d"), *SpawnLoc.ToString(), FinishedLapFrames.Num());
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

void ARacingCar::GetAllLiveryMeshes()
{
    TArray<USceneComponent*> AllChildrenComponents;
    CarSkeletalMesh->GetChildrenComponents(true, AllChildrenComponents);

    for (USceneComponent* Child : AllChildrenComponents)
    {
        if (UStaticMeshComponent* StaticMesh = Cast<UStaticMeshComponent>(Child))
        {
            if (StaticMesh)
            {
                int NumMaterials = StaticMesh->GetNumMaterials();
                for (int MatIndex = 0; MatIndex < NumMaterials; MatIndex++)
                {
                    if (StaticMesh->GetMaterial(MatIndex) == TargetMaterial)
                    {
                        MeshesWithLiveryMaterial.Add(StaticMesh);
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
    SurfaceFriction = 0.0f;
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

            if (PhysMat && PhysMat->SurfaceType == SURFACE_Gravel)
            {
                if (Bone == WheelBones[2])
                {
                    if (!bPreviousGravelRB)
                    {
                        WheelRBParticles->Activate(true);
                        bPreviousGravelRB = true;
                    }
                }

                if (Bone == WheelBones[3])
                {
                    if (!bPreviousGravelLB)
                    {
                        WheelLBParticles->Activate(true);
                        bPreviousGravelLB = true;
                    }
                }
            }
            else
            {
                if (Bone == WheelBones[2] && bPreviousGravelRB)
                {
                    WheelRBParticles->Deactivate();
                    bPreviousGravelRB = false;
                }

                if (Bone == WheelBones[3] && bPreviousGravelLB)
                {
                    WheelLBParticles->Deactivate();
                    bPreviousGravelLB = false;
                }
            }

            FVector SpringDirection = FVector::UpVector;
            FVector TireWorldVelocity = CarSkeletalMesh->GetPhysicsLinearVelocityAtPoint(Start);

            float Offset = 50.0f - Hit.Distance;

            float Vel = FVector::DotProduct(SpringDirection, TireWorldVelocity);
            float Force = (Offset * SpringStrength) - (Vel * SpringDamping);

            CarSkeletalMesh->AddForceAtLocation(SpringDirection * Force, Start);

            if (PhysMat)
            {
                SurfaceFriction += PhysMat->Friction;
            }
            else
            {
                SurfaceFriction = 4.0f;
            }
            
        }

    }

    SurfaceFriction = SurfaceFriction / 4.0f;

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
