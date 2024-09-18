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
    OnPawnMovementFinished.Broadcast();
}

void UCCPawnManagerComponent::ChangePositionChecker()
{
    if (!bIsMovementTimerActive)
    {
        TargetCell = GetTargetCellIndex(PawnCurrentCellIndex, 1, bIsMovingOnFinish);

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

    if (SelectedPawn->GetFirstBoardCellIndex() != TargetCellIndex)
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
    if (TargetCellData.FirstPawnOnCell == nullptr)
        return true;

    if (TargetCellData.FirstPawnOnCell->Tags[0] != Pawn->Tags[0] && !TargetCellData.FirstPawnOnCell->GetIsInGates())
        return true;

    if (Pawn->GetCurrentPawnPosition() != EPawnPosition::OnBoard && !TargetCellData.FirstPawnOnCell->GetIsInGates())
        return true;

    if (Pawn->GetCurrentPawnPosition() == EPawnPosition::OnBoard && Pawn->GetFirstBoardCellIndex() != CellIndex)
        if (!TargetCellData.FirstPawnOnCell->GetIsInGates())
            return true;

    if (Pawn->GetCurrentPawnPosition() == EPawnPosition::OnBoard && Pawn->GetFirstBoardCellIndex() == CellIndex)
        if (!TargetCellData.FirstPawnOnCell->GetIsInGates() && CheckFirstFinishCellIsEmpty(Pawn))
            return true;

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
            return false;
        }
        FCellsData TargetCellData = GameState->GetCellData(TargetCellIndex);

        if (TargetCellData.FirstPawnOnCell && TargetCellData.FirstPawnOnCell->GetIsInGates())
        {
            return false;
        }
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

    SetupTargetPositions(SelectedPawn->GetStartCellIndex(), StartLocation);
    SetupTargetPositions(SelectedPawn->GetFirstBoardCellIndex(), TargetLocation);

    // Run only ChangePawnPosition because we need to move Pawn only one time
    StepsToMove = 1;
    MoveDuration = 1.0f;
    GetWorld()->GetTimerManager().SetTimer(PawnMovementTimerHandle, this, &UCCPawnManagerComponent::ChangePawnPosition, 0.033f, true);
}

void UCCPawnManagerComponent::MovePawnOnBoard()
{
    int32 Index = GetTargetCellIndex(PawnCurrentCellIndex, StepsToMove, false);

    if (!CheckCanMoveToTargetCell(SelectedPawn, Index))
        return;

    if (!CheckPawnPath(SelectedPawn, StepsToMove))
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

    GetWorld()->GetTimerManager().SetTimer(MovementCheckerTimerHandle, this, &UCCPawnManagerComponent::ChangePositionChecker, 0.033f, true);
}

void UCCPawnManagerComponent::MovePawnToFinish()
{
    bShouldMoveToFinish = false;

    SetupTargetPositions(PawnCurrentCellIndex, StartLocation);
    SetupTargetPositions(SelectedPawn->GetFirstFinishCellIndex(), TargetLocation);

    SelectedPawn->SetCurrentPawnPosition(EPawnPosition::OnFinish);
    SelectedPawn->SetCurrentCellIndex(SelectedPawn->GetFirstFinishCellIndex());
    SetNewPawnDataInGameState(SelectedPawn->GetFirstFinishCellIndex(), SelectedPawn, nullptr);

    // Run only ChangePawnPosition because we need to move Pawn only one time
    StepsAlredyMoved = 0;
    StepsToMove = 1;
    MoveDuration = 1.0f;
    GetWorld()->GetTimerManager().SetTimer(PawnMovementTimerHandle, this, &UCCPawnManagerComponent::ChangePawnPosition, 0.033f, true);
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

void UCCPawnManagerComponent::BuildGates()
{
    SelectedPawn->SetIsInGates(true);
    SecondaryPawn->SetIsInGates(true);

    SetNewPawnDataInGameState(SecondaryPawn->GetCurrentCellIndex(), SelectedPawn, SecondaryPawn);

    FCellsData PreviusCellData = GameState->GetCellData(GetTargetCellIndex(SelectedPawn->GetCurrentCellIndex(), -1, false));
    FCellsData CurrentCellData = GameState->GetCellData(SelectedPawn->GetCurrentCellIndex());

    FVector Direction = PreviusCellData.CellPosition - CurrentCellData.CellPosition;
    Direction.Normalize();

    FVector Perpendicular = FVector::CrossProduct(Direction, FVector::UpVector);
    Perpendicular.Normalize();

    FVector RightPosition = CurrentCellData.CellPosition + Perpendicular * 150;
    FVector LeftPosition = CurrentCellData.CellPosition - Perpendicular * 150;

    SelectedPawn->SetActorLocation(RightPosition);
    SecondaryPawn->SetActorLocation(LeftPosition);

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
