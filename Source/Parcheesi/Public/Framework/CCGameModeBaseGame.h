// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CCCoreTypes.h"
#include "CCGameModeBaseGame.generated.h"

class ACCGameStateGame;
class APlayerController;

UCLASS()
class PARCHEESI_API ACCGameModeBaseGame : public AGameModeBase
{
    GENERATED_BODY()
public:
    ACCGameModeBaseGame();

    void StartNewGame();
    void ChangePlayerTag(FUniqueNetIdRepl PlayerNetId, FName PlayerTag);

protected:
    UPROPERTY()
    TArray<FPlayersTurnData> PlayersTurnData;

    virtual void PostLogin(APlayerController* NewPlayer) override;
    virtual void BeginPlay() override;

    void AddPlayerToAllPlayersData(FUniqueNetIdRepl PlayerNetId, FName PlayerTag);

    void UpdatePlayersTurnData();
    void UpdatePlayersTurnWidgets();
};
