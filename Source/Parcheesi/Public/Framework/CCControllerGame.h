// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CCCoreTypes.h"
#include "CCControllerGame.generated.h"

class ACCHUDGame;
class ACCPlayerPawnGame;
class UInputMappingContext;
class UInputAction;

UCLASS()
class PARCHEESI_API ACCControllerGame : public APlayerController
{
    GENERATED_BODY()

public:
    UFUNCTION(Client, Reliable)
    void Client_StartGameFromController();

    UFUNCTION(Client, Reliable)
    void Client_UpdateTurnWidgets(const TArray<FPlayersTurnData>& PlayersTurnData);

    UFUNCTION(Client, Reliable)
    void Client_ShowTurnButtonsWidget();

    UFUNCTION(Client, Reliable)
    void Client_HideTurnButtonsWidget();

    UFUNCTION(Client, Reliable)
    void Client_SwitchQuickMenuVisibility();

    UFUNCTION(Client, Reliable)
    void Client_EnableEndTurnButton();

    UFUNCTION(Client, Reliable)
    void Client_SwitchMovePawnButtonIsEnabled(const bool State);

    UFUNCTION(Client, Reliable)
    void Client_SetDiceSideOnUI(const int32 Side);

    UFUNCTION(Client, Reliable)
    void Client_ShowWinWidget(const FText& WinnerName);

    UFUNCTION(Client, Reliable)
    void Client_UpdateLobbySelection(const TArray<FAllPlayersData>& AllPlayersData);

    UFUNCTION(Client, Reliable)
    void Client_UpdateLobbySettings(const FGameSettings GameSettings);

    UFUNCTION(Client, Reliable)
    void Client_UpdatePlayersList(const TArray<FLobbyPlayersData>& AllPlayers);

    UFUNCTION(Client, Reliable)
    void Client_CreateLobbyWidget();

protected:
    UPROPERTY()
    ACCHUDGame* OwningHUD;

    UPROPERTY()
    ACCPlayerPawnGame* PlayerPawn;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputMappingContext* GameMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputMappingContext* LevelInputContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* QuickMenuAction;

    virtual void BeginPlay() override;

    virtual void SetupInputComponent() override;
};
