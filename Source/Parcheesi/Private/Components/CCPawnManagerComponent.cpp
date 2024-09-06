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
    PawnCurrentCellIndex = Pawn->GetCurrentCellIndex();

    EPawnPosition Position = Pawn->GetCurrentPawnPosition();
    switch (Position)
    {
    case EPawnPosition::OnStart:
        MovePawnFromStart();
        break;
    case EPawnPosition::OnBoard:
        MovePawnOnBoard(Steps);
        break;
    case EPawnPosition::OnFinish:
        MovePawnOnFinish();
        break;
    }
}

int32 UCCPawnManagerComponent::GetTargetCellIndex(int32 Steps)
{
    TArray<int32> CellsIds;
    GameState->GetAllCellsData().GetKeys(CellsIds);

    int32 TargetCell = 0;
    int32 MaxBoardCell = 0;
    UE_LOG(LogTemp, Display, TEXT("Current Cell: %d "), PawnCurrentCellIndex);

    for (int32 CellId : CellsIds)
    {
        if (CellId > 100) // Hardcoded. Board cells ids are between 0-99, Start Cells ids are between 100-999, Finish cells 1000-9999
            continue;
        MaxBoardCell++;
    }

    if (PawnCurrentCellIndex + Steps <= MaxBoardCell)
    {
        return PawnCurrentCellIndex + Steps;
    }
    else
    {
        TargetCell = Steps - (MaxBoardCell - PawnCurrentCellIndex);
    }

    return TargetCell;
}

void UCCPawnManagerComponent::MovePawnFromStart()
{
    FCellsData TargetCellData = GameState->GetCellData(SelectedPawn->GetFirstBoardCellIndex());
    StartLocation = SelectedPawn->GetActorLocation();
    TargetLocation = TargetCellData.CellPosition;
    StepsToMove = 1;

    SelectedPawn->SetCurrentPawnPosition(EPawnPosition::OnBoard);
    SelectedPawn->SetCurrentCellIndex(SelectedPawn->GetFirstBoardCellIndex());
    
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
        UE_LOG(LogTemp, Display, TEXT("Step %d Ended"), StepsAlredyMoved);
        StepsAlredyMoved++;

        GetWorld()->GetTimerManager().ClearTimer(PawnMovementTimerHandle);
        CurrentTime = 0.0f;
        StartLocation = FVector(0, 0, 0);
        TargetLocation = FVector(0, 0, 0);

        if (StepsAlredyMoved < StepsToMove)
        {
            SetupStartAndTargetPositions();
            GetWorld()->GetTimerManager().SetTimer(
                PawnMovementTimerHandle, this, &UCCPawnManagerComponent::ChangePawnPosition, 0.033f, true);
        }
        else if (StepsAlredyMoved == StepsToMove)
        {
            StepsAlredyMoved = 0;
            StepsToMove = 0;
            UE_LOG(LogTemp, Display, TEXT("Movement Ended "));
            return;
        }
    }
}

void UCCPawnManagerComponent::SetupStartAndTargetPositions()
{
    int32 TargetCell = GetTargetCellIndex(StepsAlredyMoved + 1);
    FCellsData TargetCellData = GameState->GetCellData(TargetCell);

    StartLocation = SelectedPawn->GetActorLocation();
    TargetLocation = TargetCellData.CellPosition;
}

void UCCPawnManagerComponent::MovePawnOnBoard(int32 Steps)
{
    int32 TargetCell = GetTargetCellIndex(Steps);
    UE_LOG(LogTemp, Display, TEXT("Target Cell: %d "), TargetCell);

    if (TargetCell == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Target Cell is Zero!"));
        return;
    }

    SetupStartAndTargetPositions();

    SelectedPawn->SetCurrentCellIndex(TargetCell);
    GetWorld()->GetTimerManager().SetTimer(PawnMovementTimerHandle, this, &UCCPawnManagerComponent::ChangePawnPosition, 0.033f, true);
}

void UCCPawnManagerComponent::MovePawnToFinish() {}

void UCCPawnManagerComponent::MovePawnOnFinish() {}