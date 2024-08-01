// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CCGameStateGame.generated.h"

UCLASS()
class PARCHEESI_API ACCGameStateGame : public AGameStateBase
{
    GENERATED_BODY()

public:
    void AddPlayerToList(FUniqueNetIdRepl PlayerNetId, FName PlayerTag);
    void ChangePlayerTag(FUniqueNetIdRepl PlayerNetId, FName PlayerTag);

    TMap<FUniqueNetIdRepl, FName> GetAllPlayersData() { return AllPlayersData; };

protected:
    UPROPERTY()
    TMap<FUniqueNetIdRepl, FName> AllPlayersData;

    void DisplayPlayersData();
};
