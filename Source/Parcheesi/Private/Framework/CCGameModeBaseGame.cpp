// Copyright Cats and Cubes. All Rights Reserved.

#include "Framework/CCGameModeBaseGame.h"
#include "Framework/CCGameStateGame.h"
#include "Framework/CCControllerGame.h"
#include "GameFramework/PlayerState.h"
#include "BoardItems/CCPawn.h"
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

    check(PawnClass);
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

    UpdatePlayersTurnData();

    SpawnPawnsOnBoard();
    StartNextTurn();
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
    TArray<ETurnColors> PlayersColors;

    ACCGameStateGame* GameStateGame = Cast<ACCGameStateGame>(GetWorld()->GetGameState());
    TMap<FUniqueNetIdRepl, FName> AllPlayersData = GameStateGame->GetAllPlayersData();
    TMap<ETurnColors, FUniqueNetIdRepl> AllPlayersTurnData = GameStateGame->GetPlayersTurnData();
    ETurnColors CurrentTurnColor = GameStateGame->GetCurrentTurnColor();
    AllPlayersTurnData.GetKeys(PlayersColors);

    for (ETurnColors Color : PlayersColors)
    {
        FPlayersTurnData PlayerData;
        
        // Set Player name
        FUniqueNetIdRepl PlayerNetId = *AllPlayersTurnData.Find(Color);
        PlayerData.PlayerName = PlayerNetId->ToString();
        
        // Set player turn status
        if (CurrentTurnColor == Color)
        {
            StartNextTurnForPlayer(PlayerNetId); // Start New turn for the player
            PlayerData.TurnSatus = true;
        }
        else
            PlayerData.TurnSatus = false;
        
        //Set player color
        switch (Color)
        {
        case ETurnColors::Red:
            PlayerData.PlayerColor = FColor::Red;
            break;
        case ETurnColors::Blue:
            PlayerData.PlayerColor = FColor::Blue;
            break;
        case ETurnColors::Yellow:
            PlayerData.PlayerColor = FColor::Yellow;
            break;
        case ETurnColors::Green:
            PlayerData.PlayerColor = FColor::Green;
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
    ETurnColors CurrentTurnColor = GameStateGame->GetCurrentTurnColor();

    uint8 TurnNumber = static_cast<uint8>(CurrentTurnColor);
    UE_LOG(LogTemp, Display, TEXT("Current color number: %d"), TurnNumber);

    UEnum* EnumPtr = StaticEnum<ETurnColors>();
    if (EnumPtr)
    {

        int64 MaxEnumValue = EnumPtr->GetMaxEnumValue() - 2; // Returns +1 then existing, and removing None element from count
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
                UE_LOG(LogTemp, Fatal, TEXT("Error in setting Next Turn Color. Do While ended with no result"));
            }

            TryIterator++;

        } while (NextColorNotFound);

        GameStateGame->SetCurrentTurnColor(CurrentTurnColor);
        uint8 NextTurn = static_cast<uint8>(CurrentTurnColor);
        UE_LOG(LogTemp, Display, TEXT("Next turn color number: %d"), NextTurn);
    }
}

void ACCGameModeBaseGame::StartNextTurnForPlayer(FUniqueNetIdRepl PlayerNetId) {
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
        TArray<AActor*> FoundSpawnCells;
        FName SpawnCellTag = UEnum::GetValueAsName(Color);

        UGameplayStatics::GetAllActorsWithTag(GetWorld(), SpawnCellTag, FoundSpawnCells);

        for (AActor* PlaceActor : FoundSpawnCells)
        {
            UE_LOG(LogTemp, Display, TEXT("Spawn Dices for %s Player. Cell Name: %s"), *SpawnCellTag.ToString(), *PlaceActor->GetName());
            FRotator Rotation;
            ACCPawn* SpawnedPawn = GetWorld()->SpawnActor<ACCPawn>(PawnClass, PlaceActor->GetActorLocation(), Rotation);
            SpawnedPawn->Multicast_SetupPawnData(Color);
        }
    }
}