// Copyright Cats and Cubes. All Rights Reserved.

#include "Framework/CCGameModeBaseGame.h"
#include "Framework/CCGameStateGame.h"
#include "GameFramework/PlayerState.h"

ACCGameModeBaseGame::ACCGameModeBaseGame() {}

void ACCGameModeBaseGame::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);
    if (NewPlayer->PlayerState)
    {
        FUniqueNetIdRepl PlayerNetId = NewPlayer->PlayerState->GetUniqueId();
        AddPlayerToAllPlayersData(PlayerNetId, "Lobby");
    }
}

void ACCGameModeBaseGame::BeginPlay() {}

void ACCGameModeBaseGame::StartNewGame()
{
    UE_LOG(LogTemp, Display, TEXT("Start game in Game Mode"));
}

void ACCGameModeBaseGame::ChangePlayerTag(FUniqueNetIdRepl PlayerNetId, FName PlayerTag)
{
    ACCGameStateGame* GameStateGame = Cast<ACCGameStateGame>(GetWorld()->GetGameState());
    GameStateGame->ChangePlayerTag(PlayerNetId, PlayerTag);
}

void ACCGameModeBaseGame::AddPlayerToAllPlayersData(FUniqueNetIdRepl PlayerNetId, FName PlayerTag)
{
    ACCGameStateGame* GameStateGame = Cast<ACCGameStateGame>(GetWorld()->GetGameState());
    GameStateGame->AddPlayerToList(PlayerNetId, PlayerTag);
}
