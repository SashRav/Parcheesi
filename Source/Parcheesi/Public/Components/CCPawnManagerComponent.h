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

    UPROPERTY()
    ACCPawn* SelectedPawn;

    UPROPERTY()
    int32 StepsToMove;

    UPROPERTY()
    ACCGameStateGame* GameState;

    void MovePawnFromStart();
    void MovePawnOnBoard();
    void MovePawnToFinish();
    void MovePawnOnFinish();
};
