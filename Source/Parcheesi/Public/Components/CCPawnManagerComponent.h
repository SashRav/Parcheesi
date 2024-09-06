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
    int32 GetTargetCellIndex(int32 Steps);

protected:
    virtual void BeginPlay() override;

    void MovePawnFromStart();
    void MovePawnOnBoard(int32 Steps);
    void MovePawnToFinish();
    void MovePawnOnFinish();

    void ChangePawnPosition();
    void SetupStartAndTargetPositions();

    UPROPERTY()
    ACCPawn* SelectedPawn;

    UPROPERTY()
    ACCGameStateGame* GameState;

    FTimerHandle PawnMovementTimerHandle;

    float Speed = 1000.0f;
    float CurrentTime;
    float MoveDuration = 1.0f;

    int32 PawnCurrentCellIndex = 0;
    int32 StepsAlredyMoved = 0;
    int32 StepsToMove = 0;

    FVector StartLocation;
    FVector TargetLocation;
};
