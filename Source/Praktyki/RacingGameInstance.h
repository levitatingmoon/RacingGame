// Copyright 2025 Teyon. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "RacingGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PRAKTYKI_API URacingGameInstance : public UGameInstance
{
	GENERATED_BODY() 

public:
	UPROPERTY()
	float TimeLimitValue = 300.0f;

	UPROPERTY()
	int LapLimitValue = 10;

	UPROPERTY()
	int MaterialIndex = 3;

	UPROPERTY()
	bool bGhostCar = false;
	
};
