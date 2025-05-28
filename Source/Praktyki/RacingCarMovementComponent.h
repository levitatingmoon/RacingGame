#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "RacingCarMovementComponent.generated.h"

UCLASS()
class PRAKTYKI_API URacingCarMovementComponent : public UPawnMovementComponent
{
    GENERATED_BODY()

public:

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

};
