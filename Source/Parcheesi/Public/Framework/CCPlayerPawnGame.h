// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CCPlayerPawnGame.generated.h"

class ACCGameModeBaseGame;
class ACCGameStateGame;
class ACCDice;

UCLASS()
class PARCHEESI_API ACCPlayerPawnGame : public APawn
{
    GENERATED_BODY()

public:
    ACCPlayerPawnGame();

    UFUNCTION(Server, Reliable)
    void Server_UpdateSelectedColor(const FName& ColorTag);

    UFUNCTION(Server, Reliable)
    void Server_EndPlayerTurn();

    UFUNCTION(Server, Reliable)
    void Server_DebugEndPlayerTurn();

    UFUNCTION(Server, Reliable)
    void Server_SpawnDice();

    UFUNCTION(Server, Reliable)
    void Server_CleanAllDices();

    UFUNCTION(Server, Reliable)
    void Server_SelectDiceSide();

    UFUNCTION(Server, Reliable)
    void Server_MoveSelectedPawn();

protected:
    virtual void BeginPlay() override;

    // Funcitons related to Dice Functionality
    void SpawnDiceActor(FVector SpawnOffest);
    void SetDiceVelocity(ACCDice* Dice);
    void MoveDicesToBoard();
    void TryDoubleDices();

    UPROPERTY()
    ACCGameModeBaseGame* ServerGameMode;

    UPROPERTY()
    ACCGameStateGame* ServerGameState;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<ACCDice> DiceClass;
};
