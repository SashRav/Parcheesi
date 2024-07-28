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

protected:
    UPROPERTY()
    TMap<FUniqueNetIdRepl, FName> AllPlayersData;
};
