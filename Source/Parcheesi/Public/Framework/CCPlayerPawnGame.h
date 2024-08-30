// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CCPlayerPawnGame.generated.h"

class ACCGameModeBaseGame;
class ACCGameStateGame;
class ACCDice;
class UInputAction;

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
    void Server_SpawnDice(bool SpawnOnBoard, bool SimulatePhysics, int32 DicesToSpawn);

    UFUNCTION(Server, Reliable)
    void Server_CleanAllDices();

    UFUNCTION(Server, Reliable)
    void Server_SelectDiceSide();

    UFUNCTION(Server, Reliable)
    void Server_MoveSelectedPawn();

    UFUNCTION(Server, Reliable)
    void Server_TryDoubleDices();

protected:
    virtual void BeginPlay() override;

    virtual void SetupPlayerInputComponent(UInputComponent* NewInputComponent) override;

    void ClickOnBoard();

    // Funcitons related to Dice Functionality
    void SpawnDiceActor(FVector SpawnOffest, bool UseVelocity, bool SimulatePhysics);
    void SetDiceVelocity(ACCDice* Dice);

    UFUNCTION()
    void MoveDicesToBoard(TArray<FVector> TargetLocations);

    UPROPERTY()
    ACCGameModeBaseGame* ServerGameMode;

    UPROPERTY()
    ACCGameStateGame* ServerGameState;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<ACCDice> DiceClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DiceMoveDelay = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName DicePlaceTag = "DicePlace";

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName DoubledDicePlaceTag = "DicePlaceDoubled";

    UPROPERTY()
    TArray<FVector> DicePlacesLocation;

    UPROPERTY()
    TArray<FVector> DoubledDicePlacesLocation;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    TMap<int32, FRotator> DiceSidesRotation;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* ClickOnBoardAction;
};
