#include "RacingCar.h"

ARacingCar::ARacingCar()
{
    PrimaryActorTick.bCanEverTick = true;

    //MovementComponent = CreateDefaultSubobject<URacingCarMovementComponent>(TEXT("MovementComponent"));
    //MovementComponent->SetUpdatedComponent(RootComponent);
}

void ARacingCar::BeginPlay()
{
    Super::BeginPlay();
}

void ARacingCar::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ARacingCar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{

}
