// Copyright Cats and Cubes. All Rights Reserved.

#include "Framework/CCGameModeBaseGame.h"
#include "Framework/CCGameStateGame.h"
#include "Framework/CCControllerGame.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "CCCoreTypes.h"

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
    UpdatePlayersTurnWidgets();
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

void ACCGameModeBaseGame::UpdatePlayersTurnData()
{
    PlayersTurnData.Empty();
    TArray<FUniqueNetIdRepl> PlayersNetId;

    ACCGameStateGame* GameStateGame = Cast<ACCGameStateGame>(GetWorld()->GetGameState());
    TMap<FUniqueNetIdRepl, FName> AllPlayersData = GameStateGame->GetAllPlayersData();
    AllPlayersData.GetKeys(PlayersNetId);

    for (FUniqueNetIdRepl NetId : PlayersNetId)
    {
        FPlayersTurnData PlayerData;
        PlayerData.PlayerName = NetId->ToString();
        PlayerData.TurnSatus = true; /// Change after adding turn data
        PlayerData.PlayerColor = FColor::White;
        /// Rework after implementing turn system
        FName* PlayerTag = AllPlayersData.Find(NetId);
        if (PlayerTag->ToString() == "Red")
            PlayerData.PlayerColor = FColor::Red;
        if (PlayerTag->ToString() == "Blue")
            PlayerData.PlayerColor = FColor::Blue;
        if (PlayerTag->ToString() == "Yellow")
            PlayerData.PlayerColor = FColor::Yellow;
        if (PlayerTag->ToString() == "Green")
            PlayerData.PlayerColor = FColor::Green;

        PlayersTurnData.Add(PlayerData);
    }
}

void ACCGameModeBaseGame::UpdatePlayersTurnWidgets()
{
    UpdatePlayersTurnData();

    TArray<FUniqueNetIdRepl> PlayersNetId;
    ACCGameStateGame* GameStateGame = Cast<ACCGameStateGame>(GetWorld()->GetGameState());
    GameStateGame->GetAllPlayersData().GetKeys(PlayersNetId);

    for (FUniqueNetIdRepl NetId : PlayersNetId)
    {
        ACCControllerGame* GameController =
            Cast<ACCControllerGame>(UGameplayStatics::GetPlayerStateFromUniqueNetId(GetWorld(), NetId)->GetOwningController());
        if (PlayersTurnData.Num() > 0)
            GameController->Client_UpdateTurnWidgets(PlayersTurnData);
    }
}
