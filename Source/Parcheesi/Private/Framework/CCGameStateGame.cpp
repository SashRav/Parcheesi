// Copyright Cats and Cubes. All Rights Reserved.

#include "Framework/CCGameStateGame.h"
#include "CCCoreTypes.h"

void ACCGameStateGame::AddPlayerToList(FUniqueNetIdRepl PlayerNetId, FName PlayerTag)
{
    UE_LOG(LogTemp, Display, TEXT("Run game state function"));
    if (PlayerTag != "")
    {
        AllPlayersData.Add(PlayerNetId, PlayerTag);
        FString UserIdSerialized = PlayerNetId.GetUniqueNetId()->ToString();
        UE_LOG(LogTemp, Display, TEXT("New Player Added. Player Id: %s, Player Tag: %s"), *UserIdSerialized, *PlayerTag.ToString());
    }
    else
        UE_LOG(LogTemp, Display, TEXT("Trying to add player with Not valide Net ID"));
    DisplayPlayersData();
}

void ACCGameStateGame::ChangePlayerTag(FUniqueNetIdRepl PlayerNetId, FName PlayerTag)
{
    AllPlayersData.Emplace(PlayerNetId, PlayerTag);
    SetPlayerTurnData();
    DisplayPlayersData();
}

void ACCGameStateGame::DisplayPlayersData()
{

    UE_LOG(LogTemp, Display, TEXT("All available players: "));

    // Debug display of all availiable players
    if (AllPlayersData.Num() > 0)
        for (auto& Elem : AllPlayersData)
        {
            const FString PlayerId = Elem.Key.GetUniqueNetId()->ToString();
            const FString Tag = Elem.Value.ToString();
            UE_LOG(LogTemp, Display, TEXT("Player Id: %s, Player Tag: %s"), *PlayerId, *Tag);
        }
}

void ACCGameStateGame::SetPlayerTurnData() {
    if (AllPlayersData.Num() > 0)
        for (auto& Elem : AllPlayersData)
        {
            const FUniqueNetIdRepl PlayerId = Elem.Key.GetUniqueNetId();
            const ETurnColors ColorEnum = GetEnumColorFromTag(Elem.Value.ToString());
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
