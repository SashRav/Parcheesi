// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CCGameLobbyUI.generated.h"

class UButton;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartGameButtonPressed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnColorButtonPressed, FName, ColorTag);

UCLASS() class PARCHEESI_API UCCGameLobbyUI : public UUserWidget
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable)
    FOnStartGameButtonPressed OnStartGameButtonPressedEvent;

    UPROPERTY(BlueprintAssignable)
    FOnColorButtonPressed OnColorButtonPressed;

protected:
    // Global lobby buttons
    UPROPERTY(meta = (BindWidget))
    class UButton* B_StartGame;

    UPROPERTY(meta = (BindWidget))
    class UButton* B_ExitLobby;

    UPROPERTY(meta = (BindWidget))
    class UButton* B_SaveSettings;

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

    UFUNCTION()
    void StartGameButtonClicked();

    UFUNCTION()
    void SelectRedButtonClicked();

    UFUNCTION()
    void SelectYellowButtonClicked();

    UFUNCTION()
    void SelectGreenButtonClicked();

    UFUNCTION()
    void SelectBlueButtonClicked();

    virtual void NativeConstruct() override;
};
