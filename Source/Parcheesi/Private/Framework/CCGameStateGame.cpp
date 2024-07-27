// Copyright Cats and Cubes. All Rights Reserved.

#include "Framework/CCGameStateGame.h"
#include "CCCoreTypes.h"

void ACCGameStateGame::AddPlayerToList(FUniqueNetIdRepl PlayerNetId, FName PlayerTag)
{
    UE_LOG(LogTemp, Display, TEXT("Run game state function"));
    Server_AddPlayerToList_Implementation(PlayerNetId, PlayerTag);

    // Server_AddPlayerToList(); // AddPlayerToList_Implementation(PlayerNetId, PlayerTag);
}

void ACCGameStateGame::BeginPlay()
{
    // Server_AddPlayerToList();
}

void ACCGameStateGame::Server_AddPlayerToList_Implementation(FUniqueNetIdRepl PlayerNetId, FName PlayerTag)
{
    UE_LOG(LogTemp, Display, TEXT("Run log on server"));
    if (PlayerTag != "")
    {
        FPlayersData PlayerData;
        PlayerData.PlayerNetId = NULL;
        PlayerData.PlayerTag = PlayerTag;

        AllPlayersData.Add(PlayerData);
        FString UserIdSerialized = PlayerNetId.GetUniqueNetId()->ToString();
        UE_LOG(LogTemp, Display, TEXT("New Player Added. Player Id: %s, Player Tag: %s"), *UserIdSerialized, *PlayerTag.ToString());
    }
    else
        UE_LOG(LogTemp, Display, TEXT("Trying to add player with Not valide Net ID"));

    UE_LOG(LogTemp, Display, TEXT("All available players: "));

    if (AllPlayersData.Num() > 0)
        for (int32 Index = 0; Index < AllPlayersData.Num(); Index++)
        {
            FString PlayerId = AllPlayersData[Index].PlayerNetId.GetUniqueNetId()->ToString();
            UE_LOG(LogTemp, Display, TEXT("Player Id: %s, Player Tag: %s"), *PlayerId, *AllPlayersData[Index].PlayerTag.ToString());
        }

}
//
// void ACCGameStateGame::AddPlayerToList_Implementation(FUniqueNetIdRepl PlayerNetId, FName PlayerTag)
//{
//    //Server_AddPlayerToList(PlayerNetId, PlayerTag);
//}
//
// void ACCGameStateGame::Server_AddPlayerToList_Implementation(FUniqueNetIdRepl PlayerNetId, FName PlayerTag)
//{
//    //if (PlayerTag != "")
//    //{
//    //    FPlayersData PlayerData;
//    //    PlayerData.PlayerNetId = NULL;
//    //    PlayerData.PlayerTag = PlayerTag;
//
//    //    //  AllPlayersData.Add(PlayerData);
//    //    FString UserIdSerialized = PlayerNetId.GetUniqueNetId()->ToString();
//    //    UE_LOG(LogTemp, Display, TEXT("New Player Added. Player Id: %s, Player Tag: %s"), *UserIdSerialized, *PlayerTag.ToString());
//    //}
//    //else
//    //    UE_LOG(LogTemp, Display, TEXT("Trying to add player with Not valide Net ID"));
//
//    //UE_LOG(LogTemp, Display, TEXT("All available players: "));
//    //for (FPlayersData Player : AllPlayersData)
//    //{
//    //    FString PlayerId = Player.PlayerNetId.GetUniqueNetId()->ToString();
//    //    UE_LOG(LogTemp, Display, TEXT("Player Id: %s, Player Tag: %s"), *PlayerId, *Player.PlayerTag.ToString());
//    //}
//}
