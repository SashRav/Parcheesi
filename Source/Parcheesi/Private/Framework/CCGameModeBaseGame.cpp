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
    check(BotAIPawnClass);

    GameStateGame = Cast<ACCGameStateGame>(GetWorld()->GetGameState());
    GameInstance = Cast<UCCGameInstance>(GetGameInstance());

    if (GameInstance->GetIsSinglePlayer())
    {
        FTimerHandle StartSinglePlayerHandle;
        GetWorld()->GetTimerManager().SetTimer(StartSinglePlayerHandle, this, &ACCGameModeBaseGame::StartSingleplayer, 0.5f, false);
    }
}

void ACCGameModeBaseGame::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    if (GameInstance && GameInstance->GetIsSinglePlayer()) // exit if in singleplayer mode
        return;

    GameStateGame = Cast<ACCGameStateGame>(GetWorld()->GetGameState());

    AddPlayerToAllPlayersData(NewPlayer, "Lobby");

    if (GameStateGame->IsGameStarted())
        NewPlayer->StartSpectatingOnly();
}

void ACCGameModeBaseGame::StartNewGame()
{
    UE_LOG(LogTemp, Display, TEXT("Start game in Game Mode"));

    if (!GameStateGame)
        return;

    TArray<AController*> PlayersControllers;
    GameStateGame->GetAllPlayersData().GetKeys(PlayersControllers);
    GameStateGame->SetupPlayersTurnData();
    GameStateGame->SetIsGameStarted(true);

    for (AController* Controller : PlayersControllers)
    {
        if (ACCControllerGame* GameController = Cast<ACCControllerGame>(Controller))
            GameController->Client_StartGameFromController();
    }

    SetNextTurnColor();
    UpdatePlayersTurnData();
    SpawnPawnsOnBoard();
    UpdatePlayerWidget();
}

void ACCGameModeBaseGame::ChangePlayerInfo(AController* Controller, FPlayerInfo PlayerInfo)
{
    if (GameStateGame)
        GameStateGame->ChangePlayerInfo(Controller, PlayerInfo);
}

void ACCGameModeBaseGame::AddPlayerToAllPlayersData(AController* Controller, FName PlayerTag)
{
    if (GameStateGame)
        GameStateGame->AddNewPlayerToList(Controller, PlayerTag);
}

void ACCGameModeBaseGame::StartNextTurn()
{
    SetNextTurnColor();
    UpdatePlayersTurnData();
    UpdatePlayerWidget();
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

    TArray<AController*> PlayersControllers;
    GameStateGame->GetAllPlayersData().GetKeys(PlayersControllers);

    for (AController* Controller : PlayersControllers)
    {
        ACCControllerGame* GameController = Cast<ACCControllerGame>(Controller);

        if (GameController)
            GameController->Client_ShowWinWidget(FText::FromName(PlayerTagName));
    }
}

void ACCGameModeBaseGame::SaveGameSettings(FGameSettings Settings)
{
    if (GameStateGame)
        GameStateGame->SetGameSettings(Settings);
}

void ACCGameModeBaseGame::DisconnectPlayer(AController* Controller)
{
    if (!GameStateGame)
        return;

    GameStateGame->RemovePlayerFromPlayersData(Controller);

    ACCControllerGame* PlayerController = Cast<ACCControllerGame>(Controller);

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

void ACCGameModeBaseGame::DisconnectPlayerByNetID(FUniqueNetIdRepl PlayerID)
{
    AController* PlayerController = UGameplayStatics::GetPlayerStateFromUniqueNetId(GetWorld(), PlayerID)->GetOwningController();
    DisconnectPlayer(PlayerController);
}

void ACCGameModeBaseGame::StartSingleplayer()
{
    if (!GameStateGame)
        return;

    // Bot colors are hardcoded untill player will be able select color in Main menu
    if (ACCPlayerPawnGame* SpawnedBotPawn = GetWorld()->SpawnActor<ACCPlayerPawnGame>(BotAIPawnClass, FVector(0, 0, 0), FRotator(0, 0, 0)))
    {
        GameStateGame->AddNewPlayerToList(SpawnedBotPawn->GetController(), "Green");
    }
    if (ACCPlayerPawnGame* SpawnedBotPawn = GetWorld()->SpawnActor<ACCPlayerPawnGame>(BotAIPawnClass, FVector(0, 0, 0), FRotator(0, 0, 0)))
    {
        GameStateGame->AddNewPlayerToList(SpawnedBotPawn->GetController(), "Blue");
    }
    if (ACCPlayerPawnGame* SpawnedBotPawn = GetWorld()->SpawnActor<ACCPlayerPawnGame>(BotAIPawnClass, FVector(0, 0, 0), FRotator(0, 0, 0)))
    {
        GameStateGame->AddNewPlayerToList(SpawnedBotPawn->GetController(), "Yellow");
    }

    ACCControllerGame* PlayerController = Cast<ACCControllerGame>(GetWorld()->GetFirstPlayerController());
    if (!PlayerController)
        return;

    PlayerController->Client_StartGameFromController();

    GameStateGame->AddNewPlayerToList(PlayerController, "Red");

    GameStateGame->SetupPlayersTurnData();
    SpawnPawnsOnBoard();

    SetNextTurnColor();
    UpdatePlayersTurnData();
    UpdatePlayerWidget();
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

void ACCGameModeBaseGame::UpdatePlayersTurnData()
{
    if (!GameStateGame)
        return;

    PlayersTurnData.Empty();
    TArray<ETurnColors> PlayersColors;

    TMap<ETurnColors, AController*> AllPlayersDataNew = GameStateGame->GetPlayersTurnData();
    ETurnColors CurrentTurnColor = GameStateGame->GetCurrentTurnColor();
    AllPlayersDataNew.GetKeys(PlayersColors);

    for (ETurnColors Color : PlayersColors)
    {
        FPlayersTurnData PlayerData;

        // Set Player name
        AController* PlayerController = *AllPlayersDataNew.Find(Color);
        PlayerData.PlayerName = PlayerController->GetName();

        // Set Local player tag
        ACCPlayerPawnGame* PlayerPawn = Cast<ACCPlayerPawnGame>(PlayerController->GetPawn());

        if (PlayerPawn)
            PlayerPawn->SetPlayerTagName(UEnum::GetValueAsName(Color));

        // Set player turn status
        if (CurrentTurnColor == Color)
        {
            StartNextTurnForPlayer(PlayerController); // Start New turn for the player
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

void ACCGameModeBaseGame::StartNextTurnForPlayer(AController* Controller)
{
    ACCControllerGame* PlayerController = Cast<ACCControllerGame>(Controller);

    if (PlayerController)
        PlayerController->Client_ShowTurnButtonsWidget();
}

void ACCGameModeBaseGame::UpdatePlayerWidget()
{
    if (!GameStateGame || PlayersTurnData.Num() == 0)
        return;

    TArray<AController*> PlayersControllers;
    GameStateGame->GetAllPlayersData().GetKeys(PlayersControllers);

    for (AController* Controller : PlayersControllers)
    {
        if (ACCControllerGame* PlayerController = Cast<ACCControllerGame>(Controller))
            PlayerController->Client_UpdateTurnWidgets(PlayersTurnData);
    }
}
