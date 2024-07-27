// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CCPlayerPawnGame.generated.h"

//class ACCGameModeBaseGame;
class ACCControllerGame;

UCLASS()
class PARCHEESI_API ACCPlayerPawnGame : public APawn
{
    GENERATED_BODY()

public:
    ACCPlayerPawnGame();

protected:
   /* UPROPERTY()
    ACCGameModeBaseGame* GameMode;*/

    UPROPERTY()
    ACCControllerGame* PlayerController;

    //UPROPERTY()
    //FUniqueNetIdRepl PlayerNetId;

    virtual void BeginPlay() override;

    //UFUNCTION(Server, Reliable)
    //void Server_AddPlayerToList();

    //UFUNCTION(NetMulticast, Reliable)
    //void Client_GetPlayerId();
};
