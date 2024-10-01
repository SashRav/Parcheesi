// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CCPawnAIController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMovementFinished, bool, bResult);

UCLASS()
class PARCHEESI_API ACCPawnAIController : public AAIController
{
    GENERATED_BODY()

public:
    virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result);

    UPROPERTY(BlueprintAssignable)
    FOnMovementFinished OnMovementFinished;
};
