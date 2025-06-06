// Copyright 2025 Teyon. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GhostFrame.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FGhostFrame
{
    GENERATED_BODY()

    UPROPERTY()
    FVector Location;

    UPROPERTY()
    FRotator Rotation;

    FGhostFrame() : Location(FVector::ZeroVector), Rotation(FRotator::ZeroRotator) {}
    FGhostFrame(FVector InLoc, FRotator InRot) : Location(InLoc), Rotation(InRot) {}
};