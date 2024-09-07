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
    MoveDuration = (1 + 0.4 * (Steps - 1)) / Steps; // Get time based on steps amount: 1 step = 1 second, 6 steps = 3 seconds

    UE_LOG(LogTemp, Display, TEXT("MoveDuration: %f"), MoveDuration);

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

int32 UCCPawnManagerComponent::GetTargetCellIndex(int32 Steps, bool bIsOnFinish)
{
    if (bIsOnFinish)
        return PawnCurrentCellIndex + Steps;

    TArray<int32> CellsIds;
    GameState->GetAllCellsData().GetKeys(CellsIds);

    int32 TargetCellToMove = 0;
    int32 MaxBoardCell = 0;

    for (int32 CellId : CellsIds)
    {
        if (CellId > 99) // Hardcoded. Board cells ids are between 0-99, Start Cells ids are between 100-999, Finish cells 1000-9999
            continue;
        MaxBoardCell++;
    }
    if (PawnCurrentCellIndex + Steps <= MaxBoardCell)
    {
        TargetCellToMove = PawnCurrentCellIndex + Steps;
    }
    else
    {
        TargetCellToMove = Steps - (MaxBoardCell - PawnCurrentCellIndex);
    }

    return TargetCellToMove;
}

void UCCPawnManagerComponent::SetupTargetPositions(int32 TargetCellIndex, FVector& StartLocationRef)
{
    FCellsData TargetCellData = GameState->GetCellData(TargetCellIndex);
    StartLocationRef = TargetCellData.CellPosition;
}

void UCCPawnManagerComponent::FinishPawnMovement()
{
    if (SecondaryPawn)
    {
        if (bShouldAttackEnemy)
        {
            MovePawnToSpawn(SecondaryPawn);
            return;
        }
        if (bShouldBuildGates)
        {
            BuildGates();
            return;
        }
    }
    UE_LOG(LogTemp, Display, TEXT("FinishPawnMovement"));
    StepsToMove = 0;
    StepsAlredyMoved = 0;
    PawnCurrentCellIndex = 0;
    TargetCell = -1;
    CurrentTime = 0.0f;
    MoveDuration = 1.0f;
    StartLocation = FVector(0, 0, 0);
    TargetLocation = FVector(0, 0, 0);
    SelectedPawn = nullptr;
    SecondaryPawn = nullptr;
    bShouldMoveToFinish = false;
    bIsMovingOnFinish = false;
    bIsMovementTimerActive = false;
    bShouldBuildGates = false;
    bShouldAttackEnemy = false;
}

void UCCPawnManagerComponent::ChangePositionChecker()
{
    if (!bIsMovementTimerActive)
    {
        TargetCell = GetTargetCellIndex(1, bIsMovingOnFinish);

        SetupTargetPositions(PawnCurrentCellIndex, StartLocation);
        SetupTargetPositions(TargetCell, TargetLocation);

        PawnCurrentCellIndex = TargetCell;
        bIsMovementTimerActive = true;

        GetWorld()->GetTimerManager().SetTimer(PawnMovementTimerHandle, this, &UCCPawnManagerComponent::ChangePawnPosition, 0.033f, true);
    }
}

void UCCPawnManagerComponent::ChangePawnPosition()
{
    CurrentTime += 0.033f; // Value from 1/30 instead of using DeltaTime
    float Alpha = FMath::Clamp(CurrentTime / MoveDuration, 0.0f, 1.0f);
    FVector NewLocation = FMath::Lerp(StartLocation, TargetLocation, Alpha);

    SelectedPawn->SetActorLocation(NewLocation);

    if (CurrentTime >= MoveDuration)
    {
        GetWorld()->GetTimerManager().ClearTimer(PawnMovementTimerHandle);
        bIsMovementTimerActive = false;
        StepsAlredyMoved++;
        CurrentTime = 0.0f;
        TargetCell = -1;
        StartLocation = FVector(0, 0, 0);
        TargetLocation = FVector(0, 0, 0);

        UE_LOG(LogTemp, Display, TEXT("StepsAlredyMoved: %d,  StepsToMove: %d"), StepsAlredyMoved, StepsToMove);

        if (StepsAlredyMoved >= StepsToMove)
        {
            GetWorld()->GetTimerManager().ClearTimer(MovementCheckerTimerHandle);

            if (bShouldMoveToFinish)
                MovePawnToFinish();
            else
                FinishPawnMovement();
        }
    }
}

bool UCCPawnManagerComponent::CheckIsTargetCellLeadsToFinish(int32 TargetCellIndex)
{
    if (SelectedPawn->GetCurrentPawnPosition() != EPawnPosition::OnBoard)
        return false;

    if (SelectedPawn->GetFirstBoardCellIndex() == TargetCellIndex)
        return true;
    else
        return false;
}

bool UCCPawnManagerComponent::CheckCanMoveToTargetCell(ACCPawn* Pawn, int32 CellIndex)
{
    FCellsData TargetCellData = GameState->GetCellData(CellIndex);
    if (TargetCellData.FirstPawnOnCell == nullptr)
        return true;

    if (TargetCellData.FirstPawnOnCell->Tags[0] != Pawn->Tags[0])
        return true;

    if (!TargetCellData.FirstPawnOnCell->GetIsInGates())
        return true;

    return false;
}

ACCPawn* UCCPawnManagerComponent::GetPawnOnCell(int32 CellIndex)
{
    FCellsData TargetCellData = GameState->GetCellData(CellIndex);
    return TargetCellData.FirstPawnOnCell;
}

bool UCCPawnManagerComponent::InterractWithTargetCellPawn(int32 CellIndex)
{
    FCellsData TargetCellData = GameState->GetCellData(CellIndex);
    if (TargetCellData.FirstPawnOnCell == nullptr)
        return true;
    if (TargetCellData.FirstPawnOnCell->Tags[0] != SelectedPawn->Tags[0])
    {
        SecondaryPawn = TargetCellData.FirstPawnOnCell;
        bShouldAttackEnemy = true;
        return true;
    }
    if (!TargetCellData.FirstPawnOnCell->GetIsInGates())
    {
        SecondaryPawn = TargetCellData.FirstPawnOnCell;
        bShouldBuildGates = true;
        return true;
    }
    return false;
}

void UCCPawnManagerComponent::MovePawnFromStart()
{
    if (!CheckCanMoveToTargetCell(SelectedPawn, SelectedPawn->GetFirstBoardCellIndex()))
        return;

    if (!InterractWithTargetCellPawn(SelectedPawn->GetFirstBoardCellIndex()))
        return;

    SetupTargetPositions(SelectedPawn->GetStartCellIndex(), StartLocation);
    SetupTargetPositions(SelectedPawn->GetFirstBoardCellIndex(), TargetLocation);

    SelectedPawn->SetCurrentPawnPosition(EPawnPosition::OnBoard);
    SelectedPawn->SetCurrentCellIndex(SelectedPawn->GetFirstBoardCellIndex());
    SetNewPawnDataInGameState(SelectedPawn->GetFirstBoardCellIndex(), SelectedPawn, nullptr);

    // Run only ChangePawnPosition because we need to move Pawn only one time
    StepsToMove = 1;
    MoveDuration = 1.0f;
    GetWorld()->GetTimerManager().SetTimer(PawnMovementTimerHandle, this, &UCCPawnManagerComponent::ChangePawnPosition, 0.033f, true);
}

void UCCPawnManagerComponent::MovePawnOnBoard()
{
    int32 Index = GetTargetCellIndex(StepsToMove, false);

    if (!CheckCanMoveToTargetCell(SelectedPawn, Index))
        return;

    if (!InterractWithTargetCellPawn(Index))
        return;

    if (SelectedPawn->GetIsInGates())
    {
        DestroyGates();
    }
    else
    {
        SetNewPawnDataInGameState(SelectedPawn->GetCurrentCellIndex(), nullptr, nullptr); // Clean previus cell if is not in gates
    }

    SetNewPawnDataInGameState(Index, SelectedPawn, nullptr);
    SelectedPawn->SetCurrentCellIndex(Index);
    
    if (CheckIsTargetCellLeadsToFinish(Index))
    {
        bShouldMoveToFinish = true;
    }

    GetWorld()->GetTimerManager().SetTimer(MovementCheckerTimerHandle, this, &UCCPawnManagerComponent::ChangePositionChecker, 0.033f, true);
}

void UCCPawnManagerComponent::MovePawnToFinish()
{
    bShouldMoveToFinish = false;

    SetupTargetPositions(PawnCurrentCellIndex, StartLocation);
    SetupTargetPositions(SelectedPawn->GetFirstFinishCellIndex(), TargetLocation);

    SelectedPawn->SetCurrentPawnPosition(EPawnPosition::OnFinish);
    SelectedPawn->SetCurrentCellIndex(SelectedPawn->GetFirstFinishCellIndex());

    // Run only ChangePawnPosition because we need to move Pawn only one time
    StepsAlredyMoved = 0;
    StepsToMove = 1;
    MoveDuration = 1.0f;
    GetWorld()->GetTimerManager().SetTimer(PawnMovementTimerHandle, this, &UCCPawnManagerComponent::ChangePawnPosition, 0.033f, true);
}

void UCCPawnManagerComponent::MovePawnOnFinish()
{
    int32 Index = GetTargetCellIndex(StepsToMove, true);
    SelectedPawn->SetCurrentCellIndex(Index);
    bIsMovingOnFinish = true;

    GetWorld()->GetTimerManager().SetTimer(MovementCheckerTimerHandle, this, &UCCPawnManagerComponent::ChangePositionChecker, 0.033f, true);
}

void UCCPawnManagerComponent::MovePawnToSpawn(ACCPawn* Pawn)
{
    SetupTargetPositions(Pawn->GetCurrentCellIndex(), StartLocation);
    SetupTargetPositions(Pawn->GetStartCellIndex(), TargetLocation);

    SelectedPawn = SecondaryPawn;
    SecondaryPawn = nullptr;

    SelectedPawn->SetCurrentPawnPosition(EPawnPosition::OnStart);
    SelectedPawn->SetCurrentCellIndex(SelectedPawn->GetStartCellIndex());
    StepsToMove = 1;
    MoveDuration = 1.0f;

    GetWorld()->GetTimerManager().SetTimer(PawnMovementTimerHandle, this, &UCCPawnManagerComponent::ChangePawnPosition, 0.033f, true);
}

void UCCPawnManagerComponent::SetNewPawnDataInGameState(int32 CellIndex, ACCPawn* FirstPawnToAdd, ACCPawn* SecondPawnToAdd)
{
    GameState->ChangeCellsDataItem(CellIndex, FirstPawnToAdd, SecondPawnToAdd);
}

void UCCPawnManagerComponent::BuildGates() {
    SelectedPawn->SetIsInGates(true);
    SecondaryPawn->SetIsInGates(true);

    SetNewPawnDataInGameState(SelectedPawn->GetCurrentCellIndex(), SelectedPawn, SecondaryPawn);

    FVector NewLocation = SelectedPawn->GetActorLocation();
    NewLocation.X += 100.0f;
    SelectedPawn->SetActorLocation(NewLocation);

    NewLocation.X += -100.0f;
    SecondaryPawn->SetActorLocation(NewLocation);

    SecondaryPawn = nullptr;
    FinishPawnMovement();
}

void UCCPawnManagerComponent::DestroyGates() 
{
    FCellsData TargetCellData = GameState->GetCellData(SelectedPawn->GetCurrentCellIndex());
    
    TargetCellData.FirstPawnOnCell->SetIsInGates(false);
    TargetCellData.SecondPawnOnCell->SetIsInGates(false);

    if (TargetCellData.SecondPawnOnCell == SelectedPawn)
    {
        TargetCellData.FirstPawnOnCell->SetActorLocation(TargetCellData.CellPosition);
        SetNewPawnDataInGameState(SelectedPawn->GetCurrentCellIndex(), TargetCellData.FirstPawnOnCell, nullptr);
        return;
    }
    if (TargetCellData.FirstPawnOnCell == SelectedPawn)
    {
        TargetCellData.SecondPawnOnCell->SetActorLocation(TargetCellData.CellPosition);
        SetNewPawnDataInGameState(SelectedPawn->GetCurrentCellIndex(), TargetCellData.SecondPawnOnCell, nullptr);
        return;
    }
}
