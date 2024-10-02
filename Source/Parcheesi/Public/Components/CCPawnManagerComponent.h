// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CCPawnManagerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPawnMovementFinished);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameFinished);

class ACCPawn;
class ACCGameStateGame;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PARCHEESI_API UCCPawnManagerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCCPawnManagerComponent();

    void MoveSelectedPawn(ACCPawn* Pawn, int32 Steps);
    bool CheckCanMoveToTargetCell(ACCPawn* Pawn, int32 CellIndex);
    bool CheckPawnCanMove(ACCPawn* Pawn, int32 Steps);
    bool CheckPawnPath(ACCPawn* Pawn, int32 Steps);

    UPROPERTY(BlueprintAssignable)
    FOnPawnMovementFinished OnPawnMovementFinished;

    UPROPERTY(BlueprintAssignable)
    FOnGameFinished OnGameFinished;

protected:
    virtual void BeginPlay() override;

    void MovePawnFromStart();
    void MovePawnOnBoard();
    void MovePawnToFinish();
    void MovePawnOnFinish();

    void ChangePawnPosition(ACCPawn* PawnToMove, TArray<FVector> Positions, bool bHandleMovement = true);

    void SetupTargetPositions(int32 TargetCellIndex, FVector& StartLocationRef);
    bool CheckIsTargetCellLeadsToFinish(int32 TargetCellIndex);
    bool CheckFirstFinishCellIsEmpty(ACCPawn* Pawn);
    bool CheckIfCanMoveOnFinishCells(int32 TargetCellIndex);

    void AttackPawnOnCell();
    void FinishPawnAttack();
    void SetNewPawnDataInGameState(int32 CellIndex, ACCPawn* FirstPawnToAdd, ACCPawn* SecondPawnToAdd);
    void FinishPawnMovement();

    void BuildGates();
    void DestroyGates();

    void CheckVictory();

    int32 GetTargetCellIndex(int32 CurrentCellIndex, int32 Steps, bool bIsOnFinish);

    ACCPawn* GetPawnOnCell(int32 CellIndex);
    bool InterractWithTargetCellPawn(int32 CellIndex);

    UPROPERTY()
    ACCPawn* SelectedPawn;

    UPROPERTY()
    ACCPawn* SecondaryPawn;

    UPROPERTY()
    ACCGameStateGame* GameState;

    UPROPERTY()
    TArray<FVector> TargetPositions;

    UFUNCTION()
    void HandlePawnFinishedMovement(bool bResult);

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

    bool bShouldBuildGates = false;
    bool bShouldAttackEnemy = false;
};
