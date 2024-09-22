// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CCCoreTypes.h"
#include "CCGameLobbyUI.generated.h"

class UButton;
class UTextBlock;
class UCheckBox;
class USlider;
class UScrollBox;
class UCCLobbyPlayerItem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartGameButtonPressed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExitToMenuFromLobbyButtonPressed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnColorButtonPressed, FName, ColorTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSaveSettingsButtonPressed, FGameSettings, GameSettings);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKickPlayerFromLobby, FUniqueNetIdRepl, PlayerID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReadyButtonPressed, bool, bIsReady);

UCLASS() class PARCHEESI_API UCCGameLobbyUI : public UUserWidget
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable)
    FOnStartGameButtonPressed OnStartGameButtonPressedEvent;

    UPROPERTY(BlueprintAssignable)
    FOnColorButtonPressed OnColorButtonPressed;

    UPROPERTY(BlueprintAssignable)
    FOnReadyButtonPressed OnReadyButtonPressed;

    UPROPERTY(BlueprintAssignable)
    FOnSaveSettingsButtonPressed OnSaveSettingsButtonPressed;

    UPROPERTY(BlueprintAssignable)
    FOnExitToMenuFromLobbyButtonPressed OnExitToMenuButtonPressed;

    UPROPERTY(BlueprintAssignable)
    FOnKickPlayerFromLobby OnKickPlayerFromLobby;

    void UpdateSelectionStatus(const TArray<FAllPlayersData>& AllPlayersData);
    void UpdateSettings(FGameSettings GameSettings);
    void UpdatePlayersList(const TArray<FUniqueNetIdRepl>& AllPlayers);

protected:
    virtual void NativeConstruct() override;
    void UpdateSelectionPlayerName(FText PlayerName, FName Tag, FText IsReadyText);
    void UpdateReadyPlayers(const TArray<FAllPlayersData>& AllPlayersData);

    // Global lobby buttons
    UPROPERTY(meta = (BindWidget))
    class UButton* B_StartGame;

    UPROPERTY(meta = (BindWidget))
    class UButton* B_ExitLobby;

    UPROPERTY(meta = (BindWidget))
    class UButton* B_SaveSettings;

    UPROPERTY(meta = (BindWidget))
    class UButton* B_ReadyToGame;

    UPROPERTY(meta = (BindWidget))
    class UButton* B_NotReadyToGame;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* T_PlayersReady;

    // Pawn selecting buttons

    UPROPERTY(meta = (BindWidget))
    class UButton* B_SelectRed;

    UPROPERTY(meta = (BindWidget))
    class UButton* B_SelectYellow;

    UPROPERTY(meta = (BindWidget))
    class UButton* B_SelectGreen;

    UPROPERTY(meta = (BindWidget))
    class UButton* B_SelectBlue;

    UPROPERTY(meta = (BindWidget))
    class UButton* B_BotRed;

    UPROPERTY(meta = (BindWidget))
    class UButton* B_BotYellow;

    UPROPERTY(meta = (BindWidget))
    class UButton* B_BotGreen;

    UPROPERTY(meta = (BindWidget))
    class UButton* B_BotBlue;

    // Pawn selecting text boxes

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* T_RedPlayerName;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* T_GreenPlayerName;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* T_YellowPlayerName;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* T_BluePlayerName;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* T_RedPlayerReady;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* T_GreenPlayerReady;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* T_YellowPlayerReady;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* T_BluePlayerReady;

    // Settings UI
    UPROPERTY(meta = (BindWidget))
    class USlider* S_RoundsToBonus;

    UPROPERTY(meta = (BindWidget))
    class USlider* S_DicesCount;

    UPROPERTY(meta = (BindWidget))
    class UCheckBox* C_MoveFromStart;

    UPROPERTY(meta = (BindWidget))
    class UCheckBox* C_CatMode;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* T_DiceCount;

    // Players List
    UPROPERTY(meta = (BindWidget))
    class UScrollBox* SB_PlayersList;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UCCLobbyPlayerItem> LobbyPlayerItemClass;

    UFUNCTION()
    void StartGameButtonClicked();

    UFUNCTION()
    void ReadyButtonClicked();

    UFUNCTION()
    void NotReadyButtonClicked();

    UFUNCTION()
    void SelectRedButtonClicked();

    UFUNCTION()
    void SelectYellowButtonClicked();

    UFUNCTION()
    void SelectGreenButtonClicked();

    UFUNCTION()
    void SelectBlueButtonClicked();

    UFUNCTION()
    void SaveSettingsButtonClicked();

    UFUNCTION()
    void DicesToUseCountChanged(float Value);

    UFUNCTION()
    void MoveFromStartChecked(bool bIsChecked);

    UFUNCTION()
    void DisconnectCurrentPlayer();

    UFUNCTION()
    void DisconnectPlayerByID(FUniqueNetIdRepl PlayerNetID);
};
