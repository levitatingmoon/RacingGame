// Copyright 2025 Teyon. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StartingSpot.generated.h"

UCLASS()
class PRAKTYKI_API AStartingSpot : public AActor
{
	GENERATED_BODY()
	
public:	

	AStartingSpot();

protected:

	virtual void BeginPlay() override;

public:	

	virtual void Tick(float DeltaTime) override;

};
