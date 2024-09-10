// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CCPlayerPawnGame.generated.h"

class ACCGameModeBaseGame;
class ACCGameStateGame;
class ACCControllerGame;
class UInputAction;
class UCCDiceComponent;
class UCCSelectItem;
class UCCPawnManagerComponent;
class ACCPawn;
class ACCDice;

UCLASS()
class PARCHEESI_API ACCPlayerPawnGame : public APawn
{
    GENERATED_BODY()

public:
    ACCPlayerPawnGame();

    void SetPlayerTagName(FName TagName) { PlayerTagName = TagName; };
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION(Server, Reliable)
    void Server_UpdateSelectedColor(const FName& ColorTag);

    UFUNCTION(Server, Reliable)
    void Server_EndPlayerTurn();

    UFUNCTION(Server, Reliable)
    void Server_DebugEndPlayerTurn();

    UFUNCTION(Server, Reliable)
    void Server_CleanAllDices();

    UFUNCTION(Server, Reliable)
    void Server_MoveSelectedPawn();

    UFUNCTION(Server, Reliable)
    void Server_RollDices();

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_SetCurrentTurn(bool Turn);

    UFUNCTION(Server, Reliable)
    void Server_CleanSelectionData();

protected:
    virtual void BeginPlay() override;

    virtual void SetupPlayerInputComponent(UInputComponent* NewInputComponent) override;

    void ClickOnBoard();
    void UpdateSelectedDiceOnUI();

    bool bIsPawnMoving = false;

    UFUNCTION(Server, Reliable)
    void Server_SwitchMovePawnButtonIsEnabled(bool State);

    UFUNCTION(Server, Reliable)
    void Server_SelectDiceActor(ACCDice* HitDice);

    UFUNCTION(Server, Reliable)
    void Server_SelectPawnActor(ACCPawn* HitPawn);

    UFUNCTION(Client, Reliable)
    void Client_EnableTurnButton();

    UFUNCTION(Client, Reliable)
    void Client_VisualSelectActor(UCCSelectItem* Component, UMeshComponent* Mesh);

    UFUNCTION(Client, Reliable)
    void Client_VisualDeselectActor(UCCSelectItem* Component, UMeshComponent* Mesh);

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_HandlePawnMovementFinished();

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_HandlePawnMovementStarted();

    UPROPERTY()
    bool bCurrentTurn = false;

    UPROPERTY(Replicated)
    FName PlayerTagName;

    UPROPERTY()
    ACCGameModeBaseGame* ServerGameMode;

    UPROPERTY()
    ACCGameStateGame* ServerGameState;

    UPROPERTY()
    ACCControllerGame* OwningPlayerController;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UCCDiceComponent* DiceComponent;

    UPROPERTY(EditAnywhere)
    UCCPawnManagerComponent* PawnManagerComponent;

    UPROPERTY(EditAnywhere)
    UCCSelectItem* SelectItemDiceComponent;

    UPROPERTY(EditAnywhere)
    UCCSelectItem* SelectItemPawnComponent;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* ClickOnBoardAction;

    UPROPERTY()
    ACCDice* SelectedDiceActor;

    UPROPERTY()
    ACCPawn* SelectedPawnActor;
};
