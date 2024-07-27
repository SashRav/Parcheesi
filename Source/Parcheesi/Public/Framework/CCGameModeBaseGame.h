// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CCGameModeBaseGame.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameStarted);

class ACCGameStateGame;
class APlayerController;

UCLASS()
class PARCHEESI_API ACCGameModeBaseGame : public AGameModeBase
{
    GENERATED_BODY()
public:
   /* UPROPERTY(BlueprintAssignable)
    FOnGameStarted OnGameStartedEvent;*/

    ACCGameModeBaseGame();

    void StartNewGame();

    

protected:
    UPROPERTY()
    ACCGameStateGame* GameStateGame;

    UPROPERTY()
    TArray<APlayerController*> Controllers;

    UFUNCTION(Server, Reliable)
    void Server_AddPlayerToAllPlayersData(FUniqueNetIdRepl PlayerNetId, FName PlayerTag);

    virtual void PostLogin(APlayerController* NewPlayer) override;  
    virtual void BeginPlay() override;
};
