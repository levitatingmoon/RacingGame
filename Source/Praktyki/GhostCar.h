// Copyright 2025 Teyon. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GhostFrame.h"
#include "GhostCar.generated.h"


/**
 * 
 */
UCLASS()
class PRAKTYKI_API AGhostCar : public APawn
{
	GENERATED_BODY()

public:
    AGhostCar();

protected:
    virtual void BeginPlay() override;

private:
    TArray<FGhostFrame> GhostFrames;
    float PlaybackTime;
    int32 CurrentFrameIndex;
    float FrameInterval;
    float PlaybackStartTime = 0.0f;
    bool bPlaying;

    void UpdateGhostPlayback(float DeltaTime);
    void ChangeMaterial();
	
public:
    virtual void Tick(float DeltaTime) override;

    void LoadGhostData(TArray<FGhostFrame>& SavedFrames);
    void StartGhostPlayback();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
    UMaterialInterface* GhostMaterial;

    UPROPERTY()
    USkeletalMeshComponent* CarSkeletalMesh;
};
