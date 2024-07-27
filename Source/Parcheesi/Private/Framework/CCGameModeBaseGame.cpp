// Copyright Cats and Cubes. All Rights Reserved.

#include "Framework/CCGameModeBaseGame.h"
#include "Framework/CCGameStateGame.h"
#include "Framework/CCControllerGame.h"
#include "GameFramework/PlayerState.h"

ACCGameModeBaseGame::ACCGameModeBaseGame() {}

void ACCGameModeBaseGame::PostLogin(APlayerController* NewPlayer) {
    Super::PostLogin(NewPlayer);
    //Controllers.Add(NewPlayer);
    if (NewPlayer->PlayerState)
    {
        FUniqueNetIdRepl PlayerNetId = NewPlayer->PlayerState->GetUniqueId(); // GetPlayerState()->GetUniqueId();
        Server_AddPlayerToAllPlayersData(PlayerNetId, "Lobby");
    }
    /*ACCControllerGame* JoinedPlayerController = Cast<ACCControllerGame>(NewPlayer);
    if (JoinedPlayerController)
    {
        UE_LOG(LogTemp, Display, TEXT("New Player joinied"));
        AddPlayerToAllPlayersData(JoinedPlayerController->GetNetId(), "Lobby");
    }*/
    
}

void ACCGameModeBaseGame::BeginPlay()
{
    if (GetWorld())
    {
        GameStateGame = Cast<ACCGameStateGame>(GetWorld()->GetGameState());
    }
}

void ACCGameModeBaseGame::StartNewGame()
{
    UE_LOG(LogTemp, Display, TEXT("Start game in Game Mode"));
    //for (APlayerController* Controller : Controllers)
    //{
    //    ACCControllerGame* JoinedPlayerController = Cast<ACCControllerGame>(Controller);
    //    if (JoinedPlayerController)
    //    {
    //        UE_LOG(LogTemp, Display, TEXT("New Player joinied"));
    //        //AddPlayerToAllPlayersData(JoinedPlayerController->GetNetId(), "Lobby");
    //    }
    //}
}

void ACCGameModeBaseGame::Server_AddPlayerToAllPlayersData_Implementation(FUniqueNetIdRepl PlayerNetId, FName PlayerTag)
{
    GameStateGame->Server_AddPlayerToList(PlayerNetId, PlayerTag);
}
