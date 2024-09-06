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
    UE_LOG(LogTemp, Display, TEXT("MoveSelectedPawn"));
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

void UCCPawnManagerComponent::MovePawnFromStart()
{
    UE_LOG(LogTemp, Display, TEXT("MovePawnFromStart"));
    if (!SelectedPawn)
        return;

    SelectedPawn->SplienComponent->ClearSplinePoints(true);

    FCellsData TargetCellData = GameState->GetCellData(SelectedPawn->GetFirstBoardCellIndex());

    SelectedPawn->SplienComponent->AddSplinePoint(SelectedPawn->GetActorLocation(), ESplineCoordinateSpace::World, true);
    SelectedPawn->SplienComponent->AddSplinePoint(TargetCellData.CellPosition, ESplineCoordinateSpace::World, true);

    StartLocation = SelectedPawn->SplienComponent->GetSplinePointAt(0, ESplineCoordinateSpace::World).Position;
    TargetLocation = SelectedPawn->SplienComponent->GetSplinePointAt(1, ESplineCoordinateSpace::World).Position;

    GetWorld()->GetTimerManager().SetTimer(PawnMovementTimerHandle, this, &UCCPawnManagerComponent::ChangePawnPosition, 0.033f, true);
}

void UCCPawnManagerComponent::ChangePawnPosition()
{
    if (!SelectedPawn)
        return;

    CurrentTime += 0.033f; // Value from 1/30 instead of using DeltaTime
    float Alpha = FMath::Clamp(CurrentTime / MoveDuration, 0.0f, 1.0f);
    FVector NewLocation = FMath::Lerp(StartLocation, TargetLocation, Alpha);

    SelectedPawn->SetActorLocation(NewLocation);

    if (CurrentTime >= MoveDuration)
    {
        GetWorld()->GetTimerManager().ClearTimer(PawnMovementTimerHandle);
        CurrentTime = 0.0f;
        StartLocation = FVector(0, 0, 0);
        TargetLocation = FVector(0, 0, 0);
    }
}

void UCCPawnManagerComponent::MovePawnOnBoard() {}

void UCCPawnManagerComponent::MovePawnToFinish() {}

void UCCPawnManagerComponent::MovePawnOnFinish() {}