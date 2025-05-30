// Copyright 2025 Teyon. All Rights Reserved.


#include "TimingLine.h"
#include "PraktykiGameModeBase.h"
#include "RacingCar.h"

// Sets default values
ATimingLine::ATimingLine()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<UBoxComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Root->OnComponentBeginOverlap.AddDynamic(this, &ATimingLine::BeginOverlap);
}

// Called when the game starts or when spawned
void ATimingLine::BeginPlay()
{
	Super::BeginPlay();

	GameMode = Cast<APraktykiGameModeBase>(GetWorld()->GetAuthGameMode());
}

// Called every frame
void ATimingLine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATimingLine::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	FString Msg = FString::Printf(TEXT("OVERLAP"));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, Msg);
	ARacingCar* RacingCar = Cast<ARacingCar>(OtherActor);
	if (RacingCar)
	{
		Msg = FString::Printf(TEXT("QUALI TIME: %.2f"), GameMode->QualiTime);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, Msg);
		RacingCar->StoreCheckpointTime(SectorNumber, GameMode->QualiTime);
	}
}