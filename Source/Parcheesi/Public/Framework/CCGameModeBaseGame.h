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
class ACCPlayerPawnGame;

UCLASS()
class PARCHEESI_API ACCGameModeBaseGame : public AGameModeBase
{
    GENERATED_BODY()

public:
   void StartNewGame();
   void StartNextTurn();

    void ChangePlayerInfo(AController* Controller, FPlayerInfo PlayerInfo);

    void FinishGame(FName PlayerTagName);

    void SaveGameSettings(FGameSettings Settings);
    void DisconnectPlayer(AController* Controller);

    void SetNextTurnColor();

protected:
    virtual void PostLogin(APlayerController* NewPlayer) override;
    virtual void BeginPlay() override;

    void AddPlayerToAllPlayersData(AController* Controller, FName PlayerTag);
    void SpawnPawnsOnBoard();


    void UpdatePlayerWidget();
    void UpdatePlayersTurnData();
    void StartNextTurnForPlayer(AController* Controller);

        // Single player setup
    void StartSingleplayer();


    UPROPERTY()
    TArray<FPlayersTurnData> PlayersTurnData;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<ACCPawn> PawnClass;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<ACCPlayerPawnGame> BotAIPawnClass;

    UPROPERTY()
    ACCGameStateGame* GameStateGame;

    UPROPERTY()
    UCCGameInstance* GameInstance;
};
