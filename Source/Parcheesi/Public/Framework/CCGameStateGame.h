// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CCCoreTypes.h"
#include "CCGameStateGame.generated.h"

class ACCDice;

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

    void AddSpawnedDice(ACCDice* Dice) { SpawnedDices.Add(Dice); }
    void RemoveDice(ACCDice* Dice) { SpawnedDices.Remove(Dice); }
    TArray<ACCDice*> GetSpawnedDices() { return SpawnedDices; }
    void CleanSpawnedDicesArray() { SpawnedDices.Empty(); }


protected:
    UPROPERTY()
    TMap<FUniqueNetIdRepl, FName> AllPlayersData;

    UPROPERTY()
    TMap<ETurnColors, FUniqueNetIdRepl> PlayersTurnData;

    UPROPERTY()
    ETurnColors CurrentTurnColor = ETurnColors::Blue;

    UPROPERTY()
    TArray<ACCDice*> SpawnedDices;

    void DisplayPlayersData();
    void SetPlayerTurnData();
    ETurnColors GetEnumColorFromTag(FString PlayerTag);
};
