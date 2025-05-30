// Copyright 2025 Teyon. All Rights Reserved.


#include "StartingSpot.h"

// Sets default values
AStartingSpot::AStartingSpot()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AStartingSpot::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AStartingSpot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

