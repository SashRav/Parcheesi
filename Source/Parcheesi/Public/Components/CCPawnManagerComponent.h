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

    UPROPERTY()
    ACCPawn* SelectedPawn;

    UPROPERTY()
    int32 StepsToMove;

    UPROPERTY()
    ACCGameStateGame* GameState;

    UPROPERTY()
    FTimerHandle PawnMovementTimerHandle;

    float Speed = 1000.0f;
    float CurrentTime;
    float MoveDuration = 1.0f;

    FVector StartLocation;
    FVector TargetLocation;
};
