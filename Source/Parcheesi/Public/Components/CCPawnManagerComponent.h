// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CCPawnManagerComponent.generated.h"

class ACCPawn;
class ACCGameStateGame;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PARCHEESI_API UCCPawnManagerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCCPawnManagerComponent();

    void MoveSelectedPawn(ACCPawn* Pawn, int32 Steps);

protected:
    virtual void BeginPlay() override;

    void MovePawnFromStart();
    void MovePawnOnBoard();
    void MovePawnToFinish();
    void MovePawnOnFinish();

    void ChangePawnPosition();
    void ChangePositionChecker();

    void SetupTargetPositions(int32 TargetCellIndex, FVector& StartLocationRef);
    bool CheckIsTargetCellLeadsToFinish(int32 TargetCellIndex);

    void FinishPawnMovement();

    int32 GetTargetCellIndex(int32 Steps, bool bIsOnFinish);

    UPROPERTY()
    ACCPawn* SelectedPawn;

    UPROPERTY()
    ACCGameStateGame* GameState;

    FTimerHandle PawnMovementTimerHandle;
    FTimerHandle MovementCheckerTimerHandle;

    float CurrentTime = 0.0f;
    float MoveDuration = 1.0f;

    int32 PawnCurrentCellIndex = 0;
    int32 StepsAlredyMoved = 0;
    int32 StepsToMove = 0;

    int32 TargetCell = -1;

    FVector StartLocation;
    FVector TargetLocation;

    bool bShouldMoveToFinish = false;
    bool bIsMovingOnFinish = false;
    bool bIsMovementTimerActive = false;
};
