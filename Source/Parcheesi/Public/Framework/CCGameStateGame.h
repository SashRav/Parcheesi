// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CCCoreTypes.h"
#include "CCGameStateGame.generated.h"

UCLASS()
class PARCHEESI_API ACCGameStateGame : public AGameStateBase
{
    GENERATED_BODY()

public:
    void AddPlayerToList(FUniqueNetIdRepl PlayerNetId, FName PlayerTag);
    void ChangePlayerTag(FUniqueNetIdRepl PlayerNetId, FName PlayerTag);

    TMap<FUniqueNetIdRepl, FName> GetAllPlayersData() { return AllPlayersData; };
    TMap<ETurnColors, FUniqueNetIdRepl> GetPlayersTurnData() { return PlayersTurnData; }

    void SetCurrentTurnColor(ETurnColors Color) { CurrentTurnColor = Color; };
    ETurnColors GetCurrentTurnColor() { return CurrentTurnColor; }

protected:
    UPROPERTY()
    TMap<FUniqueNetIdRepl, FName> AllPlayersData;

    UPROPERTY()
    TMap<ETurnColors, FUniqueNetIdRepl> PlayersTurnData;

    UPROPERTY()
    ETurnColors CurrentTurnColor = ETurnColors::Blue;

    void DisplayPlayersData();
    void SetPlayerTurnData();
    ETurnColors GetEnumColorFromTag(FString PlayerTag);
};
