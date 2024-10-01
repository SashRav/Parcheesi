// Copyright Cats and Cubes. All Rights Reserved.

#include "AI/CCPawnAIController.h"
#include "Navigation/PathFollowingComponent.h"

void ACCPawnAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
    Super::OnMoveCompleted(RequestID, Result);

    if (CurrentPosition <= MaxPosition)
    {
        MoveToNextPoint();
    }
    else
    {
        OnMovementFinished.Broadcast(Result.IsSuccess());
        TargetPositions.Empty();
    }
}

void ACCPawnAIController::MovePawnThroughCells(const TArray<FVector> Positions)
{
    UE_LOG(LogTemp, Display, TEXT("MovePawnThroughCells"));

    TargetPositions.Empty();
    CurrentPosition = 0;
    MaxPosition = 0;

    if (Positions.Num() == 0)
    {
        OnMovementFinished.Broadcast(false);
        return;
    }

    TargetPositions = Positions;
    MaxPosition = TargetPositions.Num() - 1;

    MoveToNextPoint();
}

void ACCPawnAIController::MoveToNextPoint()
{
    UE_LOG(LogTemp, Display, TEXT("MoveToNextPoint"));

    if (!TargetPositions.IsValidIndex(CurrentPosition))
    {
        OnMovementFinished.Broadcast(false);
        return;
    }

    MoveToLocation(TargetPositions[CurrentPosition], 10.0f, false, false, true, false);
    CurrentPosition++;
}