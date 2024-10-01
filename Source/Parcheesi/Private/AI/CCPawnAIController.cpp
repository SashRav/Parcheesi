// Copyright Cats and Cubes. All Rights Reserved.

#include "AI/CCPawnAIController.h"
#include "Navigation/PathFollowingComponent.h"

void ACCPawnAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
    Super::OnMoveCompleted(RequestID, Result);
    UE_LOG(LogTemp, Display, TEXT("Movement Ended"));

    OnMovementFinished.Broadcast(Result.IsSuccess());
}
