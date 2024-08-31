// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CCPlayerPawnGame.generated.h"

class ACCGameModeBaseGame;
class ACCGameStateGame;
class ACCControllerGame;
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
    void Server_SpawnDice(FVector SpawnOffset, FRotator Rotation, bool SpawnOnBoard, bool SimulatePhysics);

    UFUNCTION(Server, Reliable)
    void Server_CleanAllDices();

    UFUNCTION(Server, Reliable)
    void Server_SelectDiceSide();

    UFUNCTION(Server, Reliable)
    void Server_MoveSelectedPawn();

    UFUNCTION(Server, Reliable)
    void Server_TryDoubleDices();

    UFUNCTION(Server, Reliable)
    void Server_RollDices();

    UFUNCTION(Client, Reliable)
    void Client_EnableTurnButton();

protected:
    virtual void BeginPlay() override;

    virtual void SetupPlayerInputComponent(UInputComponent* NewInputComponent) override;

    void ClickOnBoard();
    void UpdateSelectedDiceOnUI();

    // Funcitons related to Dice Functionality
    void SpawnDiceActor(FVector SpawnOffest, FRotator Rotation, bool UseVelocity, bool SimulatePhysics);
    void SetDiceVelocity(ACCDice* Dice);

    UFUNCTION()
    void MoveDicesToBoard(TArray<FVector> TargetLocations);

    UPROPERTY()
    ACCGameModeBaseGame* ServerGameMode;

    UPROPERTY()
    ACCGameStateGame* ServerGameState;

    UPROPERTY()
    ACCControllerGame* OwningPlayerController;

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

    UPROPERTY()
    ACCDice* SelectedDiceActor;
};
