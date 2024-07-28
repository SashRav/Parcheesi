// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
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

protected:
    UPROPERTY()
    TArray<APlayerController*> Controllers;

    void AddPlayerToAllPlayersData(FUniqueNetIdRepl PlayerNetId, FName PlayerTag);

    virtual void PostLogin(APlayerController* NewPlayer) override;
    virtual void BeginPlay() override;
};
