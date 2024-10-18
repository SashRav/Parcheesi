// Copyright Cats and Cubes. All Rights Reserved.

#include "Framework/CCGameModeBaseGame.h"
#include "Framework/CCGameStateGame.h"
#include "Framework/CCControllerGame.h"
#include "Framework/CCPlayerPawnGame.h"
#include "Framework/CCGameInstance.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/SpectatorPawn.h"
#include "BoardItems/CCPawn.h"
#include "BoardItems/CCCellStart.h"
#include "Kismet/GameplayStatics.h"
#include "CCCoreTypes.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineSessionInterface.h"

void ACCGameModeBaseGame::BeginPlay()
{
    Super::BeginPlay();

    check(PawnClass);

    GameStateGame = Cast<ACCGameStateGame>(GetWorld()->GetGameState());
    GameInstance = Cast<UCCGameInstance>(GetGameInstance());

    if (GameInstance->GetIsSinglePlayer())
        StartSingleplayer();
}

void ACCGameModeBaseGame::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    if (GameInstance && GameInstance->GetIsSinglePlayer()) // exit if in singleplayer mode
        return;

    GameStateGame = Cast<ACCGameStateGame>(GetWorld()->GetGameState());

    if (!NewPlayer->PlayerState)
        return;

    FUniqueNetIdRepl PlayerNetId = NewPlayer->PlayerState->GetUniqueId();
    AddPlayerToAllPlayersData(PlayerNetId, "Lobby");

    if (GameStateGame->IsGameStarted())
        NewPlayer->StartSpectatingOnly();
}

void ACCGameModeBaseGame::StartNewGame()
{
    UE_LOG(LogTemp, Display, TEXT("Start game in Game Mode"));

    if (!GameStateGame)
        return;

    TMap<FUniqueNetIdRepl, FPlayerInfo> AllPlayersdata = GameStateGame->GetAllPlayersData();
    GameStateGame->SetupPlayersTurnData();
    GameStateGame->SetIsGameStarted(true);

    for (TPair<FUniqueNetIdRepl, FPlayerInfo>& Elem : AllPlayersdata)
    {
        ACCControllerGame* GameController =
            Cast<ACCControllerGame>(UGameplayStatics::GetPlayerStateFromUniqueNetId(GetWorld(), Elem.Key)->GetOwningController());
        GameController->Client_StartGameFromController();
    }

    UpdatePlayersTurnData();

    SpawnPawnsOnBoard();
    StartNextTurn();
}

void ACCGameModeBaseGame::ChangePlayerInfo(FUniqueNetIdRepl PlayerNetId, FPlayerInfo PlayerInfo)
{
    if (GameStateGame)
        GameStateGame->ChangePlayerInfo(PlayerNetId, PlayerInfo);
}

void ACCGameModeBaseGame::AddPlayerToAllPlayersData(FUniqueNetIdRepl PlayerNetId, FName PlayerTag)
{
    if (GameStateGame)
        GameStateGame->AddPlayerToList(PlayerNetId, PlayerTag);
}

void ACCGameModeBaseGame::UpdatePlayersTurnData()
{
    if (!GameStateGame)
        return;

    PlayersTurnData.Empty();
    TArray<ETurnColors> PlayersColors;

    TMap<ETurnColors, FUniqueNetIdRepl> AllPlayersTurnData = GameStateGame->GetPlayersTurnData();
    ETurnColors CurrentTurnColor = GameStateGame->GetCurrentTurnColor();
    AllPlayersTurnData.GetKeys(PlayersColors);

    for (ETurnColors Color : PlayersColors)
    {
        FPlayersTurnData PlayerData;

        // Set Player name
        FUniqueNetIdRepl PlayerNetId = *AllPlayersTurnData.Find(Color);
        PlayerData.PlayerName = PlayerNetId->ToString();

        // Set Local player tag
        ACCPlayerPawnGame* PlayerPawn =
            Cast<ACCPlayerPawnGame>(UGameplayStatics::GetPlayerStateFromUniqueNetId(GetWorld(), PlayerNetId)->GetPawn());

        if (PlayerPawn)
            PlayerPawn->SetPlayerTagName(UEnum::GetValueAsName(Color));

        // Set player turn status
        if (CurrentTurnColor == Color)
        {
            StartNextTurnForPlayer(PlayerNetId); // Start New turn for the player
            PlayerData.TurnSatus = true;
        }
        else
            PlayerData.TurnSatus = false;

        // Set player color
        switch (Color)
        {
        case ETurnColors::Red:
            PlayerData.PlayerColor = FColor::Red;
            break;
        case ETurnColors::Yellow:
            PlayerData.PlayerColor = FColor::Yellow;
            break;
        case ETurnColors::Green:
            PlayerData.PlayerColor = FColor::Green;
            break;
        case ETurnColors::Blue:
            PlayerData.PlayerColor = FColor::Blue;
            break;
        }

        PlayersTurnData.Add(PlayerData);
    }
}

void ACCGameModeBaseGame::UpdatePlayersTurnWidgets()
{
    if (!GameStateGame)
        return;

    UpdatePlayersTurnData();

    TArray<FUniqueNetIdRepl> PlayersNetId;
    GameStateGame->GetAllPlayersData().GetKeys(PlayersNetId);

    for (FUniqueNetIdRepl NetId : PlayersNetId)
    {
        ACCControllerGame* PlayerController =
            Cast<ACCControllerGame>(UGameplayStatics::GetPlayerStateFromUniqueNetId(GetWorld(), NetId)->GetOwningController());
        if (!PlayerController)
            return;

        if (PlayersTurnData.Num() > 0)
            PlayerController->Client_UpdateTurnWidgets(PlayersTurnData);
    }
}

void ACCGameModeBaseGame::StartNextTurn()
{
    SetNextTurnColor();
    UpdatePlayersTurnWidgets();
}

void ACCGameModeBaseGame::SetNextTurnColor()
{
    if (!GameStateGame)
        return;

    // Set the same color if only one player
    if (GameStateGame->GetAllPlayersData().Num() == 1)
    {
        TArray<ETurnColors> PlayersColors;
        GameStateGame->GetPlayersTurnData().GetKeys(PlayersColors);
        GameStateGame->SetCurrentTurnColor(PlayersColors[0]);
        return;
    }

    ETurnColors CurrentTurnColor = GameStateGame->GetCurrentTurnColor();

    uint8 TurnNumber = static_cast<uint8>(CurrentTurnColor);
    UE_LOG(LogTemp, Display, TEXT("Current color number: %d"), TurnNumber);

    UEnum* EnumPtr = StaticEnum<ETurnColors>();
    if (EnumPtr)
    {
        int64 MaxEnumValue = EnumPtr->GetMaxEnumValue() - 1; // Returns +1 then existing
        bool NextColorNotFound = true;
        int32 TryIterator = 0;
        do
        {
            if (TurnNumber >= MaxEnumValue)
                TurnNumber = 0;
            else
                TurnNumber++;

            CurrentTurnColor = static_cast<ETurnColors>(TurnNumber);
            if (GameStateGame->GetPlayersTurnData().Contains(CurrentTurnColor))
                NextColorNotFound = false;
            if (TryIterator + 1 > MaxEnumValue)
            {
                NextColorNotFound = false;
                UE_LOG(LogTemp, Warning, TEXT("Error in setting Next Turn Color. Do While ended with no result"));
            }

            TryIterator++;

        } while (NextColorNotFound);

        GameStateGame->SetCurrentTurnColor(CurrentTurnColor);
        UE_LOG(LogTemp, Display, TEXT("Next turn For player color number: %s"), *UEnum::GetValueAsString(CurrentTurnColor));
    }
}

void ACCGameModeBaseGame::StartNextTurnForPlayer(FUniqueNetIdRepl PlayerNetId)
{
    ACCControllerGame* PlayerController =
        Cast<ACCControllerGame>(UGameplayStatics::GetPlayerStateFromUniqueNetId(GetWorld(), PlayerNetId)->GetOwningController());

    if (PlayerController)
        PlayerController->Client_ShowTurnButtonsWidget();
}

void ACCGameModeBaseGame::SpawnPawnsOnBoard()
{
    if (!GameStateGame)
        return;

    TArray<ETurnColors> PlayersColors;
    GameStateGame->GetPlayersTurnData().GetKeys(PlayersColors);

    for (ETurnColors Color : PlayersColors)
    {
        FName SpawnCellTag = UEnum::GetValueAsName(Color);

        TArray<AActor*> FoundSpawnActors;
        UGameplayStatics::GetAllActorsWithTag(GetWorld(), SpawnCellTag, FoundSpawnActors);

        for (AActor* PlaceActor : FoundSpawnActors)
        {
            ACCCellStart* SpawnCell = Cast<ACCCellStart>(PlaceActor);

            FRotator Rotation;
            ACCPawn* SpawnedPawn = GetWorld()->SpawnActor<ACCPawn>(PawnClass, PlaceActor->GetActorLocation(), Rotation);
            if (SpawnedPawn)
                SpawnedPawn->Multicast_SetupPawnData(
                    Color, SpawnCell->GetCellIndex(), SpawnCell->GetClosestBoardCellIndex(), SpawnCell->GetClosestFinishCellIndex());
        }
    }
}

void ACCGameModeBaseGame::FinishGame(FName PlayerTagName)
{
    if (!GameStateGame)
        return;

    TArray<FUniqueNetIdRepl> PlayersNetId;
    GameStateGame->GetAllPlayersData().GetKeys(PlayersNetId);

    for (FUniqueNetIdRepl NetId : PlayersNetId)
    {
        ACCControllerGame* GameController =
            Cast<ACCControllerGame>(UGameplayStatics::GetPlayerStateFromUniqueNetId(GetWorld(), NetId)->GetOwningController());

        if (GameController)
            GameController->Client_ShowWinWidget(FText::FromName(PlayerTagName));
    }
}

void ACCGameModeBaseGame::SaveGameSettings(FGameSettings Settings)
{
    if (GameStateGame)
        GameStateGame->SetGameSettings(Settings);
}

void ACCGameModeBaseGame::DisconnectPlayer(FUniqueNetIdRepl PlayerID)
{
    if (!GameStateGame)
        return;

    GameStateGame->RemovePlayerFromPlayersData(PlayerID);

    ACCControllerGame* PlayerController =
        Cast<ACCControllerGame>(UGameplayStatics::GetPlayerStateFromUniqueNetId(GetWorld(), PlayerID)->GetOwningController());

    if (!PlayerController)
        return;

    IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
    if (PlayerController->GetRemoteRole() != ROLE_AutonomousProxy)
    {
        if (!SessionInterface.IsValid())
            return;

        if (GameInstance)
        {
            SessionInterface->DestroySession(GameInstance->GetSessionName());
            GameInstance->SetIsSinglePlayer(false);
            GameInstance->SetPlayerColor(ETurnColors::None);
        }
    }

    PlayerController->ClientTravel("/Game/Maps/MenuMap", ETravelType::TRAVEL_Absolute);
}

void ACCGameModeBaseGame::StartSingleplayer() 
{
    ACCControllerGame* GameController = Cast<ACCControllerGame>(GetWorld()->GetFirstPlayerController());
    if (GameController)
        GameController->Client_StartGameFromController();
}