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
    void ChangePlayerInfo(AController* Controller, FPlayerInfo PlayerInfo);
    void ChangeCellsDataItem(int32 Index, ACCPawn* FirstPawn, ACCPawn* SecondPawn);

    FCellsData GetCellData(int32 Index) { return *CellsData.Find(Index); };
    TMap<int32, FCellsData> GetAllCellsData() { return CellsData; }
    bool CheckCellIsValidOnIndex(int32 CellIndex);

    
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

    void SetupPlayersTurnData();
    void AddNewPlayerToList(AController* Controller, FName PlayerTag);
    void RemovePlayerFromPlayersData(AController* Controller);
    TMap<AController*, FPlayerInfo> GetAllPlayersData() { return AllPlayersData; };
    TMap<ETurnColors, AController*> GetPlayersTurnData() { return PlayersTurnData; }

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
    TMap<AController*, FPlayerInfo> AllPlayersData;

    UPROPERTY()
    TMap<ETurnColors, AController*> PlayersTurnData;

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
