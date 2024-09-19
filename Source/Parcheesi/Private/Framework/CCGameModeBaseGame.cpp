// Copyright Cats and Cubes. All Rights Reserved.

#include "Framework/CCGameModeBaseGame.h"
#include "Framework/CCGameStateGame.h"
#include "Framework/CCControllerGame.h"
#include "Framework/CCPlayerPawnGame.h"
#include "GameFramework/PlayerState.h"
#include "BoardItems/CCPawn.h"
#include "BoardItems/CCCellStart.h"
#include "Kismet/GameplayStatics.h"
#include "CCCoreTypes.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineSessionInterface.h"


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

    check(PawnClass);
}

void ACCGameModeBaseGame::StartNewGame()
{
    UE_LOG(LogTemp, Display, TEXT("Start game in Game Mode"));

    TArray<FUniqueNetIdRepl> PlayersNetId;
    ACCGameStateGame* GameStateGame = Cast<ACCGameStateGame>(GetWorld()->GetGameState());
    GameStateGame->SetupPlayersTurnData();
    GameStateGame->GetAllPlayersData().GetKeys(PlayersNetId);

    for (FUniqueNetIdRepl NetId : PlayersNetId)
    {
        ACCControllerGame* GameController =
            Cast<ACCControllerGame>(UGameplayStatics::GetPlayerStateFromUniqueNetId(GetWorld(), NetId)->GetOwningController());
        GameController->Client_StartGameFromController();
    }

    UpdatePlayersTurnData();

    SpawnPawnsOnBoard();
    StartNextTurn();
}

void ACCGameModeBaseGame::ChangePlayerInfo(FUniqueNetIdRepl PlayerNetId, FPlayerInfo PlayerInfo)
{
    ACCGameStateGame* GameStateGame = Cast<ACCGameStateGame>(GetWorld()->GetGameState());
    GameStateGame->ChangePlayerInfo(PlayerNetId, PlayerInfo);
}

void ACCGameModeBaseGame::AddPlayerToAllPlayersData(FUniqueNetIdRepl PlayerNetId, FName PlayerTag)
{
    ACCGameStateGame* GameStateGame = Cast<ACCGameStateGame>(GetWorld()->GetGameState());
    GameStateGame->AddPlayerToList(PlayerNetId, PlayerTag);
}

void ACCGameModeBaseGame::UpdatePlayersTurnData()
{
    PlayersTurnData.Empty();
    TArray<ETurnColors> PlayersColors;

    ACCGameStateGame* GameStateGame = Cast<ACCGameStateGame>(GetWorld()->GetGameState());
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
    UpdatePlayersTurnData();

    TArray<FUniqueNetIdRepl> PlayersNetId;
    ACCGameStateGame* GameStateGame = Cast<ACCGameStateGame>(GetWorld()->GetGameState());
    GameStateGame->GetAllPlayersData().GetKeys(PlayersNetId);

    for (FUniqueNetIdRepl NetId : PlayersNetId)
    {
        ACCControllerGame* PlayerController =
            Cast<ACCControllerGame>(UGameplayStatics::GetPlayerStateFromUniqueNetId(GetWorld(), NetId)->GetOwningController());
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
    ACCGameStateGame* GameStateGame = Cast<ACCGameStateGame>(GetWorld()->GetGameState());

    // Set the same color if only one player
    if (GameStateGame->GetAllPlayersData().Num() == 1)
    {
        UE_LOG(LogTemp, Display, TEXT(""));
        TArray<ETurnColors> PlayersColors;
        GameStateGame->GetPlayersTurnData().GetKeys(PlayersColors);
        GameStateGame->SetCurrentTurnColor(PlayersColors[0]);
        UE_LOG(LogTemp, Display, TEXT("Set the same turn color: %s"), *UEnum::GetValueAsString(GameStateGame->GetCurrentTurnColor()));
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

    PlayerController->Client_ShowTurnButtonsWidget();
}

void ACCGameModeBaseGame::SpawnPawnsOnBoard()
{
    TArray<ETurnColors> PlayersColors;
    ACCGameStateGame* GameStateGame = Cast<ACCGameStateGame>(GetWorld()->GetGameState());
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
            SpawnedPawn->Multicast_SetupPawnData(
                Color, SpawnCell->GetCellIndex(), SpawnCell->GetClosestBoardCellIndex(), SpawnCell->GetClosestFinishCellIndex());
        }
    }
}

void ACCGameModeBaseGame::FinishGame(FName PlayerTagName)
{
    TArray<FUniqueNetIdRepl> PlayersNetId;
    ACCGameStateGame* GameStateGame = Cast<ACCGameStateGame>(GetWorld()->GetGameState());
    GameStateGame->GetAllPlayersData().GetKeys(PlayersNetId);

    for (FUniqueNetIdRepl NetId : PlayersNetId)
    {
        ACCControllerGame* GameController =
            Cast<ACCControllerGame>(UGameplayStatics::GetPlayerStateFromUniqueNetId(GetWorld(), NetId)->GetOwningController());
        GameController->Client_ShowWinWidget(FText::FromName(PlayerTagName));
    }
}

void ACCGameModeBaseGame::SaveGameSettings(FGameSettings Settings)
{
    ACCGameStateGame* GameStateGame = Cast<ACCGameStateGame>(GetWorld()->GetGameState());
    GameStateGame->SetGameSettings(Settings);
}

void ACCGameModeBaseGame::DisconnectPlayer(FUniqueNetIdRepl PlayerID)
{
    ACCControllerGame* PlayerController =
        Cast<ACCControllerGame>(UGameplayStatics::GetPlayerStateFromUniqueNetId(GetWorld(), PlayerID)->GetOwningController());

    ACCGameStateGame* GameStateGame = Cast<ACCGameStateGame>(GetWorld()->GetGameState());
    GameStateGame->RemovePlayerFromPlayersData(PlayerID);

    if (PlayerController->HasAuthority())
    {
        IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
        if (SessionInterface.IsValid())
        {
            SessionInterface->DestroySession(FName("My session")); // Hardcoded untill session creation system will be implemented properly
        }
    }

    PlayerController->ClientTravel("/Game/Maps/MenuMap", ETravelType::TRAVEL_Absolute);
}
