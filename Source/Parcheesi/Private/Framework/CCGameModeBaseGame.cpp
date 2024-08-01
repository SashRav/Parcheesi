// Copyright Cats and Cubes. All Rights Reserved.

#include "Framework/CCGameModeBaseGame.h"
#include "Framework/CCGameStateGame.h"
#include "Framework/CCControllerGame.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"

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

void ACCGameModeBaseGame::BeginPlay()
{
    Super::BeginPlay();
}

void ACCGameModeBaseGame::StartNewGame()
{
    UE_LOG(LogTemp, Display, TEXT("Start game in Game Mode"));

    TArray<FUniqueNetIdRepl> PlayersNetId;
    ACCGameStateGame* GameStateGame = Cast<ACCGameStateGame>(GetWorld()->GetGameState());
    GameStateGame->GetAllPlayersData().GetKeys(PlayersNetId);
    
    for (FUniqueNetIdRepl NetId : PlayersNetId)
    {
        ACCControllerGame* GameController =
            Cast<ACCControllerGame>(UGameplayStatics::GetPlayerStateFromUniqueNetId(GetWorld(), NetId)->GetOwningController());
        GameController->Client_StartGameFromController();
    }
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
