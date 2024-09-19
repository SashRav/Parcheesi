// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CCCoreTypes.h"
#include "CCGameModeBaseGame.generated.h"

class ACCGameStateGame;
class APlayerController;
class ACCPawn;

UCLASS()
class PARCHEESI_API ACCGameModeBaseGame : public AGameModeBase
{
    GENERATED_BODY()
public:
    ACCGameModeBaseGame();

    void StartNewGame();
    void StartNextTurn();

    void ChangePlayerInfo(FUniqueNetIdRepl PlayerNetId, FPlayerInfo PlayerInfo);
    
    void FinishGame(FName PlayerTagName);

    void SaveGameSettings(FGameSettings Settings);
    void DisconnectPlayer(FUniqueNetIdRepl PlayerID);

protected:
    UPROPERTY()
    TArray<FPlayersTurnData> PlayersTurnData;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<ACCPawn> PawnClass;

    virtual void PostLogin(APlayerController* NewPlayer) override;
    virtual void BeginPlay() override;

    void AddPlayerToAllPlayersData(FUniqueNetIdRepl PlayerNetId, FName PlayerTag);

    void UpdatePlayersTurnData();
    void UpdatePlayersTurnWidgets();

    void StartNextTurnForPlayer(FUniqueNetIdRepl PlayerNetId);
    void SetNextTurnColor();

    void SpawnPawnsOnBoard();
};
