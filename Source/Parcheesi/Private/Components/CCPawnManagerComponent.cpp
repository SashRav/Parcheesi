// Copyright Cats and Cubes. All Rights Reserved.

#include "Components/CCPawnManagerComponent.h"
#include "Components/SplineComponent.h"
#include "BoardItems/CCPawn.h"
#include "Framework/CCGameStateGame.h"
#include "Kismet/GameplayStatics.h"
#include "CCCoreTypes.h"
#include "AI/CCPawnAIController.h"

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
    TargetPositions.Empty();

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

int32 UCCPawnManagerComponent::GetTargetCellIndex(int32 CurrentCellIndex, int32 Steps, bool bIsOnFinish)
{
    if (bIsOnFinish)
        return CurrentCellIndex + Steps;

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
    if (CurrentCellIndex + Steps < 1) // Check for previus cell fo building gates if previus cell has max id
        return MaxBoardCell;

    if (CurrentCellIndex + Steps <= MaxBoardCell)
    {
        TargetCellToMove = CurrentCellIndex + Steps;
    }
    else
    {
        TargetCellToMove = Steps - (MaxBoardCell - CurrentCellIndex);
    }

    return TargetCellToMove;
}

void UCCPawnManagerComponent::SetupTargetPositions(int32 TargetCellIndex, FVector& StartLocationRef)
{
    if (!GameState)
        return;

    FCellsData TargetCellData = GameState->GetCellData(TargetCellIndex);
    StartLocationRef = TargetCellData.CellPosition;
}

void UCCPawnManagerComponent::FinishPawnMovement()
{
    if (SecondaryPawn)
    {
        if (bShouldAttackEnemy)
        {
            AttackPawnOnCell();
            return;
        }
        if (bShouldBuildGates)
        {
            BuildGates();
            return;
        }
    }
    UE_LOG(LogTemp, Display, TEXT("FinishPawnMovement"));

    CheckVictory();

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
    TargetPositions.Empty();
    OnPawnMovementFinished.Broadcast();
}

void UCCPawnManagerComponent::ChangePawnPosition(ACCPawn* PawnToMove, TArray<FVector> Positions, bool bHandleMovement)
{
    if (!PawnToMove)
        return;

    UE_LOG(LogTemp, Display, TEXT("Cells to move: %d"), Positions.Num());

    PawnToMove->SetMovementTick(true);

    ACCPawnAIController* AIController = Cast<ACCPawnAIController>(PawnToMove->GetController());
    if (AIController)
    {
        AIController->MovePawnThroughCells(Positions);
        if (bHandleMovement)
            AIController->OnMovementFinished.AddDynamic(this, &UCCPawnManagerComponent::HandlePawnFinishedMovement);
    }
}

void UCCPawnManagerComponent::HandlePawnFinishedMovement(bool bResult)
{
    UE_LOG(LogTemp, Display, TEXT("HandlePawnFinishedMovement"));

    ACCPawnAIController* AIController;
    if (SelectedPawn)
    {
        SelectedPawn->SetMovementTick(false);
        AIController = Cast<ACCPawnAIController>(SelectedPawn->GetController());
        if (AIController)
            AIController->OnMovementFinished.RemoveAll(this);
    }

    if (SecondaryPawn)
    {
        SecondaryPawn->SetMovementTick(false);
        AIController = Cast<ACCPawnAIController>(SecondaryPawn->GetController());
        if (AIController)
            AIController->OnMovementFinished.RemoveAll(this);
    }

    if (bShouldMoveToFinish)
        MovePawnToFinish();
    else
        FinishPawnMovement();
}

bool UCCPawnManagerComponent::CheckIsTargetCellLeadsToFinish(int32 TargetCellIndex)
{
    if (SelectedPawn && SelectedPawn->GetCurrentPawnPosition() != EPawnPosition::OnBoard)
        return false;

    if (SelectedPawn && SelectedPawn->GetFirstBoardCellIndex() != TargetCellIndex)
        return false;
    return true;
}

bool UCCPawnManagerComponent::CheckFirstFinishCellIsEmpty(ACCPawn* Pawn)
{
    FCellsData TargetCellData = GameState->GetCellData(Pawn->GetFirstFinishCellIndex());
    if (TargetCellData.FirstPawnOnCell != nullptr)
        return false;

    return true;
}

bool UCCPawnManagerComponent::CheckIfCanMoveOnFinishCells(int32 TargetCellIndex)
{
    if (!GameState->CheckCellIsValidOnIndex(TargetCellIndex))
        return false;

    FCellsData TargetCellData = GameState->GetCellData(TargetCellIndex);
    if (TargetCellData.FirstPawnOnCell != nullptr)
        return false;

    return true;
}

bool UCCPawnManagerComponent::CheckCanMoveToTargetCell(ACCPawn* Pawn, int32 CellIndex)
{
    FCellsData TargetCellData = GameState->GetCellData(CellIndex);

    // If cell is empty just go to it
    if (TargetCellData.FirstPawnOnCell == nullptr)
    {
        TargetPositions.Add(TargetCellData.CellPosition);
        return true;
    }

    // If cell has enemy and they are not in gates move to this cell
    if (TargetCellData.FirstPawnOnCell->Tags[0] != Pawn->Tags[0] && !TargetCellData.FirstPawnOnCell->GetIsInGates())
    {
        FCellsData PreviusCellData;

        if (Pawn->GetCurrentPawnPosition() == EPawnPosition::OnStart)
            PreviusCellData = GameState->GetCellData(Pawn->GetStartCellIndex());
        else
            PreviusCellData = GameState->GetCellData(GetTargetCellIndex(CellIndex, -1, false));

        FVector Direction = PreviusCellData.CellPosition - TargetCellData.CellPosition;
        Direction.Normalize();

        FVector TargetPoint = TargetCellData.CellPosition + Direction * 250.0f;
        TargetPositions.Add(TargetPoint);
        return true;
    }

    // If Pawn moves from start and target cell does not have gates
    if (Pawn->GetCurrentPawnPosition() != EPawnPosition::OnBoard && !TargetCellData.FirstPawnOnCell->GetIsInGates())
    {
        TargetPositions.Add(TargetCellData.CellPosition);
        return true;
    }

    // If Pawn moves on board and target cell does not further then first board cell
    if (Pawn->GetCurrentPawnPosition() == EPawnPosition::OnBoard && Pawn->GetFirstBoardCellIndex() != CellIndex)
        // If target cell does not contains gates
        if (!TargetCellData.FirstPawnOnCell->GetIsInGates())
        {
            TargetPositions.Add(TargetCellData.CellPosition);
            return true;
        }

    // If Pawn moves on board and target cell is equals to fist board cell
    if (Pawn->GetCurrentPawnPosition() == EPawnPosition::OnBoard && Pawn->GetFirstBoardCellIndex() == CellIndex)
        if (!TargetCellData.FirstPawnOnCell->GetIsInGates() && CheckFirstFinishCellIsEmpty(Pawn))
        {
            TargetPositions.Add(TargetCellData.CellPosition);
            return true;
        }

    return false;
}

bool UCCPawnManagerComponent::CheckPawnCanMove(ACCPawn* Pawn, int32 Steps)
{
    int32 LocalTargetCell = -1;

    if (Pawn->GetCurrentPawnPosition() == EPawnPosition::OnStart)
    {
        if (GameState->GetGameSettings().bMoveWithSix && Steps != 6)
            return false;
        else
            return CheckCanMoveToTargetCell(Pawn, Pawn->GetFirstBoardCellIndex());
    }
    if (Pawn->GetCurrentPawnPosition() == EPawnPosition::OnFinish)
    {
        LocalTargetCell = GetTargetCellIndex(Pawn->GetCurrentCellIndex(), Steps, true);
        return CheckIfCanMoveOnFinishCells(LocalTargetCell);
    }
    if (Pawn->GetCurrentPawnPosition() == EPawnPosition::OnBoard)
    {
        LocalTargetCell = GetTargetCellIndex(Pawn->GetCurrentCellIndex(), Steps, false);
        if (!CheckCanMoveToTargetCell(Pawn, LocalTargetCell))
        {
            return false;
        }
        if (!CheckPawnPath(Pawn, Steps))
        {
            return false;
        }
        return true;
    }
    return false;
}

bool UCCPawnManagerComponent::CheckPawnPath(ACCPawn* Pawn, int32 Steps)
{
    for (int32 Step = 1; Step < Steps; Step++)
    {
        int32 TargetCellIndex = GetTargetCellIndex(Pawn->GetCurrentCellIndex(), Step, false);
        // Check if finish cell on the path
        if (Pawn->GetFirstBoardCellIndex() == TargetCellIndex)
        {
            TargetPositions.Empty();
            return false;
        }
        FCellsData TargetCellData = GameState->GetCellData(TargetCellIndex);

        if (TargetCellData.FirstPawnOnCell && TargetCellData.FirstPawnOnCell->GetIsInGates())
        {
            TargetPositions.Empty();
            return false;
        }
        TargetPositions.Add(TargetCellData.CellPosition);
    }
    return true;
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
    if (TargetCellData.FirstPawnOnCell->Tags[0] != SelectedPawn->Tags[0] && !TargetCellData.FirstPawnOnCell->GetIsInGates())
    {
        SecondaryPawn = TargetCellData.FirstPawnOnCell;
        bShouldAttackEnemy = true;
        return true;
    }
    if (!TargetCellData.FirstPawnOnCell->GetIsInGates() && !bShouldMoveToFinish)
    {
        SecondaryPawn = TargetCellData.FirstPawnOnCell;
        bShouldBuildGates = true;
        return true;
    }
    if (bShouldMoveToFinish)
        return true;

    return false;
}

void UCCPawnManagerComponent::MovePawnFromStart()
{
    if (!CheckCanMoveToTargetCell(SelectedPawn, SelectedPawn->GetFirstBoardCellIndex()))
        return;

    if (!InterractWithTargetCellPawn(SelectedPawn->GetFirstBoardCellIndex()))
        return;

    if (!bShouldBuildGates)
        SetNewPawnDataInGameState(SelectedPawn->GetFirstBoardCellIndex(), SelectedPawn, nullptr);

    SelectedPawn->SetCurrentCellIndex(SelectedPawn->GetFirstBoardCellIndex());
    SelectedPawn->SetCurrentPawnPosition(EPawnPosition::OnBoard);

    ChangePawnPosition(SelectedPawn, TargetPositions);
}

void UCCPawnManagerComponent::MovePawnOnBoard()
{
    int32 Index = GetTargetCellIndex(PawnCurrentCellIndex, StepsToMove, false);

    if (!CheckPawnPath(SelectedPawn, StepsToMove))
        return;

    if (!CheckCanMoveToTargetCell(SelectedPawn, Index))
        return;

    bShouldMoveToFinish = CheckIsTargetCellLeadsToFinish(Index);

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

    if (!bShouldMoveToFinish)
    {
        SetNewPawnDataInGameState(Index, SelectedPawn, nullptr);
        SelectedPawn->SetCurrentCellIndex(Index);
    }
    else
    {
        if (!CheckFirstFinishCellIsEmpty(SelectedPawn))
        {
            UE_LOG(LogTemp, Display, TEXT("CheckFirstFinishCellIsEmpty is false"));
            return;
        }
    }
    ChangePawnPosition(SelectedPawn, TargetPositions);
}

void UCCPawnManagerComponent::MovePawnToFinish()
{
    bShouldMoveToFinish = false;

    SetupTargetPositions(SelectedPawn->GetFirstFinishCellIndex(), TargetLocation);
    TargetPositions.Empty();
    TargetPositions.Add(TargetLocation);

    SelectedPawn->SetCurrentPawnPosition(EPawnPosition::OnFinish);
    SelectedPawn->SetCurrentCellIndex(SelectedPawn->GetFirstFinishCellIndex());
    SetNewPawnDataInGameState(SelectedPawn->GetFirstFinishCellIndex(), SelectedPawn, nullptr);

    // Run only ChangePawnPosition because we need to move Pawn only one time
    StepsAlredyMoved = 0;
    StepsToMove = 1;
    MoveDuration = 1.0f;

    ChangePawnPosition(SelectedPawn, TargetPositions);
}

void UCCPawnManagerComponent::MovePawnOnFinish()
{
    int32 Index = GetTargetCellIndex(PawnCurrentCellIndex, StepsToMove, true);
    if (!CheckIfCanMoveOnFinishCells(Index))
        return;

    SetNewPawnDataInGameState(SelectedPawn->GetCurrentCellIndex(), nullptr, nullptr);

    SelectedPawn->SetCurrentCellIndex(Index);
    bIsMovingOnFinish = true;
    SetNewPawnDataInGameState(Index, SelectedPawn, nullptr);

    SetupTargetPositions(Index, TargetLocation);
    TargetPositions.Add(TargetLocation);

    ChangePawnPosition(SelectedPawn, TargetPositions);
}

void UCCPawnManagerComponent::AttackPawnOnCell()
{
    SelectedPawn->bIsAttacking = true;
    FTimerHandle AttackTimerHaldle;

    GetWorld()->GetTimerManager().SetTimer(AttackTimerHaldle, this, &UCCPawnManagerComponent::FinishPawnAttack, 1.0f, false);
}

void UCCPawnManagerComponent::FinishPawnAttack()
{
    SelectedPawn->bIsAttacking = false;
    SetupTargetPositions(SecondaryPawn->GetStartCellIndex(), TargetLocation);
    TargetPositions.Empty();
    TargetPositions.Add(TargetLocation);
    SecondaryPawn->SetCurrentPawnPosition(EPawnPosition::OnStart);
    SecondaryPawn->SetCurrentCellIndex(SelectedPawn->GetStartCellIndex());
    ChangePawnPosition(SecondaryPawn, TargetPositions);

    SetupTargetPositions(SelectedPawn->GetCurrentCellIndex(), TargetLocation);
    TargetPositions.Empty();
    TargetPositions.Add(TargetLocation);
    ChangePawnPosition(SelectedPawn, TargetPositions, false);

    bShouldAttackEnemy = false;
}

void UCCPawnManagerComponent::SetNewPawnDataInGameState(int32 CellIndex, ACCPawn* FirstPawnToAdd, ACCPawn* SecondPawnToAdd)
{
    GameState->ChangeCellsDataItem(CellIndex, FirstPawnToAdd, SecondPawnToAdd);
}

void UCCPawnManagerComponent::BuildGates()
{
    SetNewPawnDataInGameState(SecondaryPawn->GetCurrentCellIndex(), SelectedPawn, SecondaryPawn);

    FCellsData PreviusCellData = GameState->GetCellData(GetTargetCellIndex(SelectedPawn->GetCurrentCellIndex(), -1, false));
    FCellsData CurrentCellData = GameState->GetCellData(SelectedPawn->GetCurrentCellIndex());

    FVector Direction = PreviusCellData.CellPosition - CurrentCellData.CellPosition;
    Direction.Normalize();

    FVector Perpendicular = FVector::CrossProduct(Direction, FVector::UpVector);
    Perpendicular.Normalize();

    TArray<FVector> FirstPositions;
    TArray<FVector> SecondPositions;

    FVector RightPosition = CurrentCellData.CellPosition + Perpendicular * 150;
    FVector LeftPosition = CurrentCellData.CellPosition - Perpendicular * 150;
    FirstPositions.Add(RightPosition);
    SecondPositions.Add(LeftPosition);

    ChangePawnPosition(SelectedPawn, FirstPositions);
    ChangePawnPosition(SecondaryPawn, SecondPositions, false);

    SelectedPawn->SetIsInGates(true);
    SecondaryPawn->SetIsInGates(true);

    SecondaryPawn = nullptr;
}

void UCCPawnManagerComponent::DestroyGates()
{
    FCellsData TargetCellData = GameState->GetCellData(SelectedPawn->GetCurrentCellIndex());

    TargetCellData.FirstPawnOnCell->SetIsInGates(false);
    TargetCellData.SecondPawnOnCell->SetIsInGates(false);

    TArray<FVector> Position;
    Position.Add(TargetCellData.CellPosition);

    if (TargetCellData.SecondPawnOnCell == SelectedPawn)
    {
        ChangePawnPosition(TargetCellData.FirstPawnOnCell, Position, false);
        SetNewPawnDataInGameState(SelectedPawn->GetCurrentCellIndex(), TargetCellData.FirstPawnOnCell, nullptr);
        return;
    }
    if (TargetCellData.FirstPawnOnCell == SelectedPawn)
    {
        ChangePawnPosition(TargetCellData.SecondPawnOnCell, Position, false);
        SetNewPawnDataInGameState(SelectedPawn->GetCurrentCellIndex(), TargetCellData.SecondPawnOnCell, nullptr);
        return;
    }
}

void UCCPawnManagerComponent::CheckVictory()
{
    TArray<ACCPawn*> AllPawns = GameState->GetAllPawns();
    FName PawnTag = SelectedPawn->Tags[0];

    int32 PawnsOnFinish = 0;

    for (ACCPawn* Pawn : AllPawns)
    {
        if (Pawn->Tags[0] != PawnTag)
            continue;

        if (Pawn->GetCurrentPawnPosition() == EPawnPosition::OnFinish)
            PawnsOnFinish++;
    }
    UE_LOG(LogTemp, Display, TEXT("Pawns On Finish: %d"), PawnsOnFinish);
    if (PawnsOnFinish >= 4)
    {
        UE_LOG(LogTemp, Display, TEXT("VICTORY !!!!!!!!!!!!!!!!"));
        OnGameFinished.Broadcast();
    }
}
