// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CCCoreTypes.h"
#include "CCGameModeBaseGame.generated.h"

class ACCGameStateGame;
class UCCGameInstance;
class APlayerController;
class ACCPawn;
class ACCBotAIPawn;

UCLASS()
class PARCHEESI_API ACCGameModeBaseGame : public AGameModeBase
{
    GENERATED_BODY()

public:
    void StartNewGame();
    void StartNextTurn();

    void ChangePlayerInfo(FUniqueNetIdRepl PlayerNetId, FPlayerInfo PlayerInfo);

    void FinishGame(FName PlayerTagName);

    void SaveGameSettings(FGameSettings Settings);
    void DisconnectPlayer(FUniqueNetIdRepl PlayerID);

protected:
    virtual void PostLogin(APlayerController* NewPlayer) override;
    virtual void BeginPlay() override;

    void AddPlayerToAllPlayersData(FUniqueNetIdRepl PlayerNetId, FName PlayerTag);

    void UpdatePlayersTurnData();
    void UpdatePlayersTurnWidgets();

    void StartNextTurnForPlayer(FUniqueNetIdRepl PlayerNetId);
    void SetNextTurnColor();

    void SpawnPawnsOnBoard();

    // Single player setup
    void StartSingleplayer();

    UPROPERTY()
    TArray<FPlayersTurnData> PlayersTurnData;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<ACCPawn> PawnClass;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<ACCBotAIPawn> BotAIPawnClass;

    UPROPERTY()
    ACCGameStateGame* GameStateGame;

    UPROPERTY()
    UCCGameInstance* GameInstance;
};
