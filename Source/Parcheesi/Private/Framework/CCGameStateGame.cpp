// Copyright Cats and Cubes. All Rights Reserved.

#include "Framework/CCGameStateGame.h"
#include "BoardItems/CCPawn.h"
#include "BoardItems/CCCell.h"
#include "Kismet/GameplayStatics.h"
#include "CCCoreTypes.h"

void ACCGameStateGame::BeginPlay()
{
    Super::BeginPlay();

    SetCellsData();

    GameSettingsData.bMoveWithSix = true;
    GameSettingsData.DicesToRool = 2;
}

void ACCGameStateGame::ChangePlayerInfo(AController* Controller, FPlayerInfo PlayerInfo)
{
    AllPlayersData.Emplace(Controller, PlayerInfo);
    OnSelectingColorInLobby.Broadcast();
}

void ACCGameStateGame::ChangeCellsDataItem(int32 Index, ACCPawn* FirstPawn, ACCPawn* SecondPawn)
{
    if (Index <= 0)
        return;

    if (CellsData.Find(Index))
    {
        FCellsData Data = *CellsData.Find(Index);
        Data.FirstPawnOnCell = FirstPawn;
        Data.SecondPawnOnCell = SecondPawn;

        CellsData.Add(Index, Data);
    }
}

bool ACCGameStateGame::CheckCellIsValidOnIndex(int32 CellIndex)
{
    if (CellsData.Find(CellIndex))
        return true;

    return false;
}

void ACCGameStateGame::RemovePlayerFromPlayersData(AController* Controller)
{
    AllPlayersData.Remove(Controller);
    OnPlayersCountChanged.Broadcast();
    OnSelectingColorInLobby.Broadcast(); // Reset Ready players
}

ETurnColors ACCGameStateGame::GetEnumColorFromTag(FString PlayerTag)
{
    if (PlayerTag == "Red")
        return ETurnColors::Red;
    if (PlayerTag == "Blue")
        return ETurnColors::Blue;
    if (PlayerTag == "Yellow")
        return ETurnColors::Yellow;
    if (PlayerTag == "Green")
        return ETurnColors::Green;

    return ETurnColors::None;
}

void ACCGameStateGame::SetCellsData()
{
    TArray<AActor*> FoundCellsActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACCCell::StaticClass(), FoundCellsActors);

    if (FoundCellsActors.Num() == 0)
        return;

    for (AActor* CellActor : FoundCellsActors)
    {
        ACCCell* Cell = Cast<ACCCell>(CellActor);
        FCellsData Data;
        Data.CellPosition = Cell->GetActorLocation();
        Data.FirstPawnOnCell = nullptr;
        Data.SecondPawnOnCell = nullptr;

        CellsData.Add(Cell->GetCellIndex(), Data);
    }
}

void ACCGameStateGame::SetGameSettings(FGameSettings Settings)
{
    GameSettingsData = Settings;
    OnUpdatingSettingsInLobby.Broadcast();
}

TArray<ACCPawn*> ACCGameStateGame::GetAllPawns()
{
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACCPawn::StaticClass(), FoundActors);

    TArray<ACCPawn*> FoundPawns;

    if (FoundActors.Num() == 0)
        return FoundPawns;

    for (AActor* PawnActot : FoundActors)
    {
        FoundPawns.Add(Cast<ACCPawn>(PawnActot));
    }

    return FoundPawns;
}

void ACCGameStateGame::AddNewPlayerToList(AController* Controller, FName PlayerTag)
{
    if (Controller)
    {
        FPlayerInfo PlayerData;
        PlayerData.Tag = PlayerTag;
        PlayerData.bIsReady = false;

        AllPlayersData.Add(Controller, PlayerData);
        UE_LOG(LogTemp, Display, TEXT("New Player Added. Player: %s, Player Tag: %s"), *Controller->GetName(), *PlayerTag.ToString());
    }
    else
        UE_LOG(LogTemp, Error, TEXT("Trying to add player with Not valide controller "));

     // Update Players List in lobby
    OnPlayersCountChanged.Broadcast();
    // Update widgets for players that are alredy in lobby
    OnSelectingColorInLobby.Broadcast();
}

void ACCGameStateGame::SetupPlayersTurnData()
{
    if (AllPlayersData.Num() > 0)
        for (TPair<AController*, FPlayerInfo>& Elem : AllPlayersData)
        {
            FPlayerInfo PlayerData = Elem.Value;
            const ETurnColors ColorEnum = GetEnumColorFromTag(PlayerData.Tag.ToString());
            AController* Controller = Elem.Key;
            PlayersTurnData.Add(ColorEnum, Controller);
        }
    UE_LOG(LogTemp, Display, TEXT("Total players with colors: %d"), PlayersTurnData.Num());
}