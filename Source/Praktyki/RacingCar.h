#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "RacingCarMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "RacingCar.generated.h"

UCLASS()
class PRAKTYKI_API ARacingCar : public APawn
{
    GENERATED_BODY()

public:
    ARacingCar();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    //UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    //URacingCarMovementComponent* MovementComponent;

};
