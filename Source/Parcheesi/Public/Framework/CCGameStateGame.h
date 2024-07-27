// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CCCoreTypes.h"
#include "CCGameStateGame.generated.h"

UCLASS()
class PARCHEESI_API ACCGameStateGame : public AGameStateBase
{
    GENERATED_BODY()

public:
    /*UFUNCTION()*/
    void AddPlayerToList(FUniqueNetIdRepl PlayerNetId, FName PlayerTag);
   
    //TMap<FUniqueNetIdRepl, FName> GetAllPlayersData() { return AllPlayersData; }

     UFUNCTION(Server, Reliable)
    void Server_AddPlayerToList(FUniqueNetIdRepl PlayerNetId, FName PlayerTag);

protected:

    virtual void BeginPlay() override;

    UPROPERTY()
    TArray<FPlayersData> AllPlayersData;

   
    /*UPROPERTY()
    TMap<FUniqueNetIdRepl, FName> AllPlayersData; */
};
