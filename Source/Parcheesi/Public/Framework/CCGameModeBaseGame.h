// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CCGameModeBaseGame.generated.h"

class ACCHUDGame;
class ACCControllerGame;
class ACCGameStateGame;
class ACCPlayerStateGame;
class ACCSpectatorPawnGame;
class ACCPlayerPawnGame;

UCLASS()
class PARCHEESI_API ACCGameModeBaseGame : public AGameModeBase
{
    GENERATED_BODY()
public:
    ACCGameModeBaseGame();

    void StartNewGame();

protected:
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<ACCHUDGame> GameHUDClass;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<ACCControllerGame> PlayerControllerGameClass;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<ACCPlayerStateGame> PlayerStateGameClass;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<ACCSpectatorPawnGame> SpectatorPawnGameClass;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<ACCPlayerPawnGame> PlayerPawnGameClass;
};
