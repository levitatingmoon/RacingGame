// Copyright 2025 Teyon. All Rights Reserved.


#include "TimingLine.h"
#include "PraktykiGameModeBase.h"
#include "RacingCar.h"
#include "MyPlayerController.h"


ATimingLine::ATimingLine()
{

	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<UBoxComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Root->OnComponentBeginOverlap.AddDynamic(this, &ATimingLine::BeginOverlap);
}


void ATimingLine::BeginPlay()
{
	Super::BeginPlay();

	GameMode = Cast<APraktykiGameModeBase>(GetWorld()->GetAuthGameMode());
}


void ATimingLine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATimingLine::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	ARacingCar* RacingCar = Cast<ARacingCar>(OtherActor);
	if (RacingCar)
	{

		AMyPlayerController* PC = Cast<AMyPlayerController>(RacingCar->GetController());
		if (PC)
		{
			PC->SectorUpdate(SectorNumber, GameMode->QualiTime);
		}

	}
}