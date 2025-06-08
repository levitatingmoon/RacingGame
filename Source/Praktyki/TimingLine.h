// Copyright 2025 Teyon. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "TimingLine.generated.h"

class APraktykiGameModeBase;
class ARacingCar;

UCLASS()
class PRAKTYKI_API ATimingLine : public AActor
{
	GENERATED_BODY()
	
public:	

	ATimingLine();

protected:

	virtual void BeginPlay() override;

public:	

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Line")
	bool bIsStartLine = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Line")
	int SectorNumber = 0;
	 
	UPROPERTY(EditAnywhere)
	UBoxComponent* Root;

	APraktykiGameModeBase* GameMode;

};
