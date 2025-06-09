// Copyright 2025 Teyon. All Rights Reserved.


#include "GhostCar.h"
#include "GameFramework/Actor.h"
#include "Components/MeshComponent.h"
#include "Materials/MaterialInterface.h" 


AGhostCar::AGhostCar()
{
    PrimaryActorTick.bCanEverTick = true;
    bPlaying = false;
    PlaybackTime = 0.0f;
    CurrentFrameIndex = 0;
    FrameInterval = 0.33f; // 30 FPS
}

void AGhostCar::BeginPlay()
{
    Super::BeginPlay();

    CarSkeletalMesh = Cast<USkeletalMeshComponent>(GetRootComponent());

    if (CarSkeletalMesh)
    {
        CarSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
    ChangeMaterial();
}

void AGhostCar::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bPlaying)
    {
        UpdateGhostPlayback(DeltaTime);
    }
}

void AGhostCar::LoadGhostData(TArray<FGhostFrame>& SavedFrames)
{
    GhostFrames = SavedFrames;
}

void AGhostCar::StartGhostPlayback()
{
    if (GhostFrames.Num() > 1)
    {
        bPlaying = true;
        PlaybackTime = 0.0f;
        CurrentFrameIndex = 0;
        PlaybackStartTime = GetWorld()->GetTimeSeconds();
    }
}

void AGhostCar::UpdateGhostPlayback(float DeltaTime)
{
    if (GhostFrames.Num() < 2)
    {
        bPlaying = false;
        return;
    }

    PlaybackTime += DeltaTime;

    int32 NewFrameIndex = FMath::FloorToInt(PlaybackTime / FrameInterval);
    NewFrameIndex = FMath::Clamp(NewFrameIndex, 0, GhostFrames.Num() - 2);

    if (NewFrameIndex != CurrentFrameIndex)
    {
        CurrentFrameIndex = NewFrameIndex;
    }

    if (CurrentFrameIndex >= GhostFrames.Num() - 1)
    {
        bPlaying = false;
        return;
    }

    float Alpha = (PlaybackTime - CurrentFrameIndex * FrameInterval) / FrameInterval;

    const FGhostFrame& FrameA = GhostFrames[CurrentFrameIndex];
    const FGhostFrame& FrameB = GhostFrames[CurrentFrameIndex + 1];

    FVector InterpolatedLocation = FMath::Lerp(FrameA.Location, FrameB.Location, Alpha);
    FRotator InterpolatedRotation = FMath::Lerp(FrameA.Rotation, FrameB.Rotation, Alpha);

    SetActorLocationAndRotation(InterpolatedLocation, InterpolatedRotation, false, nullptr, ETeleportType::TeleportPhysics);
}

void AGhostCar::ChangeMaterial()
{
    TArray<UActorComponent*> AllComponents;
    GetComponents(AllComponents);

    for (UActorComponent* Component : AllComponents)
    {
        UMeshComponent* Mesh = Cast<UMeshComponent>(Component);
        if (Mesh)
        {
            int32 NumberMaterials = Mesh->GetNumMaterials();
            for (int32 i = 0; i < NumberMaterials; ++i)
            {
                Mesh->SetMaterial(i, GhostMaterial);
            }
        }
    }
}
