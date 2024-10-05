// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "Components/TimelineComponent.h"
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
class USpringArmComponent;
class UCameraComponent;
class UTimelineComponent;

UCLASS()
class PARCHEESI_API ACCPlayerPawnGame : public APawn
{
    GENERATED_BODY()

public:
    ACCPlayerPawnGame();

    void SetPlayerTagName(FName TagName);

    UFUNCTION(Server, Reliable)
    void Server_UpdateSelectedColor(const FName& ColorTag);

    UFUNCTION(Server, Reliable)
    void Server_PlayerIsReady(bool bIsReady);

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

    UFUNCTION(Server, Reliable)
    void Server_HandleGameFinished();

    UFUNCTION(Server, Reliable)
    void Server_UpdateLobbySelection();

    UFUNCTION(Server, Reliable)
    void Server_UpdateLobbySettings();

    UFUNCTION(Server, Reliable)
    void Server_UpdateLobbyPlayers();

    UFUNCTION(Server, Reliable)
    void Server_DisconnectPlayer(FUniqueNetIdRepl PlayerID);

protected:
    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void SetupPlayerInputComponent(UInputComponent* NewInputComponent) override;

    void ClickOnBoard();
    void DoubleClickOnBoard();
    void UpdateSelectedDiceOnUI();
    void InitLobby();
    void ResetPawnClickedTimes();

    // Camera control
    void ZoomCamera(const FInputActionValue& Value);
    void RotateCamera(const FInputActionValue& Value);
    void ZoomCameraFromPawn(float ZoomCameraValue);
    void ResetCameraByClick();

    UFUNCTION()
    void MoveCameraToDefaultPosition(float Value);

    UFUNCTION()
    void FinishCameraMovementToDefaultPosistion();

    UFUNCTION()
    void MoveActorToSelectedPosition(float Value);

    UFUNCTION()
    void FinishActorMovementToSelectedPosistion();

    FTimeline CameraMovementTimeline;
    FOnTimelineFloat ProgressTimelineFunction;
    FOnTimelineEvent TimelineFinishedCallback;

    const float DefaultSpringArmLenght = 5700.0f;
    const float PawnSelectedSpringArmLenght = 3000.0f;
    float CameraMoveDuration = 0.5f;
    float CameraMoveTimeElapsed = 0.0f;
    float InitialArmLength;
    float CurrentDeltaTime;

    FVector DefaultActorLocation;
    FRotator InitialActorRotation;
    FRotator InitialArmRotation;
    FRotator InitalCameraRotation;
    FRotator DefaultActorRotation = FRotator(0.0f, 0.0f, 0.0f);
    FRotator DefaultSpringArmRotation = FRotator(-70.0f, 0.0f, 0.0f);
    FRotator PawnCameraRotation = FRotator(20.0f, 0.0f, 0.0f);
    FRotator PawnSpringArmRotationTarget;
    float PawnSpringArmRotationPitch = -60.0f;

    bool bIsCameraMoving = false;
    bool bIsCameraInDefaultState = true;
    bool bIsCameraFolowPawn = false;

    bool bIsPawnMoving = false;
    bool bIsAnyPawnCanMove = false;
    bool bDicesSpawned = false;

    int32 PawnClickedTimes = 0;
    FTimerHandle ResetDoubleClickPawnHandle;
    FVector PositionFromMove;
    FVector PositionToMove;

    UFUNCTION(Server, Reliable)
    void Server_CheckIsAnyMoveAvailable();

    UFUNCTION(Server, Reliable)
    void Server_CheckIfCanEnableEndTurn();

    UFUNCTION(Server, Reliable)
    void Server_TrySwitchMovePawnButtonIsEnabled(bool State);

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

    UFUNCTION(Client, Reliable)
    void Client_UpdateLobbySelection(const TArray<FAllPlayersData>& AllPlayersData);

    UFUNCTION(Client, Reliable)
    void Client_UpdateLobbySettings(FGameSettings GameSettings);

    UFUNCTION(Client, Reliable)
    void Client_UpdateLobbyPlayers(const TArray<FUniqueNetIdRepl>& AllPlayers);

    // Camera Control
    UFUNCTION(Client, Reliable)
    void Client_ResetCameraToDefault();

    UFUNCTION(Client, Reliable)
    void Client_MoveActorToPosition(UCurveFloat* CurveToUse);

    UFUNCTION(Client, Reliable)
    void Client_SetCameraInitPosition(const FName Tag);

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

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* DoubleClickOnBoardAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* ZoomCameraAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* RotateCameraAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* ResetCameraAction;

    UPROPERTY(EditAnywhere)
    UCurveFloat* CameraMovementToDefaultCurve;

    UPROPERTY(EditAnywhere)
    UCurveFloat* CameraMovementToPawnCurve;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UCameraComponent* CameraComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    USpringArmComponent* SpringArmComponent;

    UPROPERTY()
    ACCDice* SelectedDiceActor;

    UPROPERTY(Replicated)
    ACCPawn* SelectedPawnActor;

    UPROPERTY()
    UTimelineComponent* TimelineComponent;
};
