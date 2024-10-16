// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CCCoreTypes.h"
#include "CCGameStateGame.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSelectingColorInLobby);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUpdatingSettingsInLobby);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayersCountChanged);

class ACCDice;
class ACCPawn;

UCLASS()
class PARCHEESI_API ACCGameStateGame : public AGameStateBase
{
    GENERATED_BODY()

public:
    void AddPlayerToList(FUniqueNetIdRepl PlayerNetId, FName PlayerTag);
    void ChangePlayerInfo(FUniqueNetIdRepl PlayerNetId, FPlayerInfo PlayerInfo);
    void ChangeCellsDataItem(int32 Index, ACCPawn* FirstPawn, ACCPawn* SecondPawn);
    void SetupPlayersTurnData();
    void SetupNewPlayersTurnData();
    FCellsData GetCellData(int32 Index) { return *CellsData.Find(Index); };
    TMap<int32, FCellsData> GetAllCellsData() { return CellsData; }
    bool CheckCellIsValidOnIndex(int32 CellIndex);

    TMap<FUniqueNetIdRepl, FPlayerInfo> GetAllPlayersData() { return AllPlayersData; };
    void RemovePlayerFromPlayersData(FUniqueNetIdRepl PlayerID);
    TMap<ETurnColors, FUniqueNetIdRepl> GetPlayersTurnData() { return PlayersTurnData; }

    void SetCurrentTurnColor(ETurnColors Color) { CurrentTurnColor = Color; };
    ETurnColors GetCurrentTurnColor() { return CurrentTurnColor; }

    void AddSpawnedDice(ACCDice* Dice) { SpawnedDices.Add(Dice); }
    void RemoveDice(ACCDice* Dice) { SpawnedDices.Remove(Dice); }
    TArray<ACCDice*> GetSpawnedDices() { return SpawnedDices; }
    void CleanSpawnedDicesArray() { SpawnedDices.Empty(); }

    FGameSettings GetGameSettings() { return GameSettingsData; }
    void SetGameSettings(FGameSettings Settings);

    TArray<ACCPawn*> GetAllPawns();

    bool IsGameStarted() { return bIsGameStarted; }
    void SetIsGameStarted(bool Status) { bIsGameStarted = Status; }
    TMap<ETurnColors, AController*> GetNewPlayersTurnData() { return PlayersNewTurnData; }

    // Singleplayer logic
    void AddNewPlayerToList(AController* Controller, FName PlayerTag);
    TMap<AController*, FPlayerInfo> GetAllNewPlayersData() { return AllPlayersDataNew; };

    UPROPERTY(BlueprintAssignable)
    FOnSelectingColorInLobby OnSelectingColorInLobby;

    UPROPERTY(BlueprintAssignable)
    FOnUpdatingSettingsInLobby OnUpdatingSettingsInLobby;

    UPROPERTY(BlueprintAssignable)
    FOnPlayersCountChanged OnPlayersCountChanged;

protected:
    virtual void BeginPlay() override;

    ETurnColors GetEnumColorFromTag(FString PlayerTag);

    void SetCellsData();

    UPROPERTY()
    TMap<FUniqueNetIdRepl, FPlayerInfo> AllPlayersData;

    UPROPERTY()
    TMap<AController*, FPlayerInfo> AllPlayersDataNew;

    UPROPERTY()
    TMap<ETurnColors, FUniqueNetIdRepl> PlayersTurnData;

    UPROPERTY()
    TMap<ETurnColors, AController*> PlayersNewTurnData;

    UPROPERTY()
    TMap<int32, FCellsData> CellsData;

    UPROPERTY()
    ETurnColors CurrentTurnColor = ETurnColors::None;

    UPROPERTY()
    TArray<ACCDice*> SpawnedDices;

    UPROPERTY()
    FGameSettings GameSettingsData;

    UPROPERTY()
    bool bIsGameStarted = false;
};
