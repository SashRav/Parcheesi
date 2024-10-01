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
    UPROPERTY(BlueprintAssignable)
    FOnMovementFinished OnMovementFinished;

    void MovePawnThroughCells(const TArray<FVector> Positions);

protected:
    virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result);
    void MoveToNextPoint();

    int32 CurrentPosition = 0;
    int32 MaxPosition = 0;

    UPROPERTY()
    TArray<FVector> TargetPositions;
};
