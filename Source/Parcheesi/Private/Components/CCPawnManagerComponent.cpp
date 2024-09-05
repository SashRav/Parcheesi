// Copyright Cats and Cubes. All Rights Reserved.

#include "Components/CCPawnManagerComponent.h"
#include "Components/SplineComponent.h"
#include "BoardItems/CCPawn.h"
#include "Framework/CCGameStateGame.h"
#include "Kismet/GameplayStatics.h"
#include "CCCoreTypes.h"

UCCPawnManagerComponent::UCCPawnManagerComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UCCPawnManagerComponent::BeginPlay()
{
    Super::BeginPlay();

    GameState = Cast<ACCGameStateGame>(UGameplayStatics::GetGameState(GetWorld()));
}

void UCCPawnManagerComponent::MoveSelectedPawn(ACCPawn* Pawn, int32 Steps)
{
    SelectedPawn = Pawn;
    StepsToMove = Steps;

    EPawnPosition Position = Pawn->GetCurrentPawnPosition();
    switch (Position)
    {
    case EPawnPosition::OnStart:
        MovePawnFromStart();
        break;
    case EPawnPosition::OnBoard:
        MovePawnOnBoard();
        break;
    case EPawnPosition::OnFinish:
        MovePawnOnFinish();
        break;
    }
}

void UCCPawnManagerComponent::MovePawnFromStart() {
    if (!SelectedPawn)
        return;
    SelectedPawn->SplienComponent->ClearSplinePoints(true);

    FCellsData TargetCellData = GameState->GetCellData(SelectedPawn->GetFirstBoardCellIndex());
    
    SelectedPawn->SplienComponent->AddSplinePoint(SelectedPawn->GetActorLocation(), ESplineCoordinateSpace::World, true);
    SelectedPawn->SplienComponent->AddSplinePoint(TargetCellData.CellPosition,ESplineCoordinateSpace::World,true);
}

void UCCPawnManagerComponent::MovePawnOnBoard() {}

void UCCPawnManagerComponent::MovePawnToFinish() {}

void UCCPawnManagerComponent::MovePawnOnFinish() {}