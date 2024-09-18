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

void ACCGameStateGame::AddPlayerToList(FUniqueNetIdRepl PlayerNetId, FName PlayerTag)
{
    UE_LOG(LogTemp, Display, TEXT("Run game state function"));
    if (PlayerNetId.IsValid())
    {
        FPlayerInfo PlayerData;
        PlayerData.Tag = PlayerTag;
        PlayerData.bIsReady = false;

        AllPlayersData.Add(PlayerNetId, PlayerData);
        FString UserIdSerialized = PlayerNetId.GetUniqueNetId()->ToString();
        UE_LOG(LogTemp, Display, TEXT("New Player Added. Player Id: %s, Player Tag: %s"), *UserIdSerialized, *PlayerTag.ToString());
    }
    else
        UE_LOG(LogTemp, Display, TEXT("Trying to add player with Not valide Net ID"));

    // Update Players List in lobby
    OnNewPlayerJoined.Broadcast();
    // Update widgets for players that are alredy in lobby
    OnSelectingColorInLobby.Broadcast();
}

void ACCGameStateGame::ChangePlayerInfo(FUniqueNetIdRepl PlayerNetId, FPlayerInfo PlayerInfo)
{
    AllPlayersData.Emplace(PlayerNetId, PlayerInfo);
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

void ACCGameStateGame::SetupPlayersTurnData()
{
    if (AllPlayersData.Num() > 0)
        for (TPair<FUniqueNetIdRepl, FPlayerInfo>& Elem : AllPlayersData)
        {
            FPlayerInfo PlayerData = Elem.Value;
            const FUniqueNetIdRepl PlayerId = Elem.Key.GetUniqueNetId();
            const ETurnColors ColorEnum = GetEnumColorFromTag(PlayerData.Tag.ToString());
            if (ColorEnum != ETurnColors::None)
                PlayersTurnData.Add(ColorEnum, PlayerId);
        }
    UE_LOG(LogTemp, Display, TEXT("Total players with colors: %d"), PlayersTurnData.Num());
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
    for (AActor* PawnActot : FoundActors)
    {
        FoundPawns.Add(Cast<ACCPawn>(PawnActot));
    }

    return FoundPawns;
}
