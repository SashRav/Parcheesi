// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "CCCreateGameUI.generated.h"

class UCCMenuButtonUI;
class ACCHUDMainMenu;
class FOnlineSessionSettings;
class UEditableTextBox;
class UCheckBox;

UCLASS() class PARCHEESI_API UCCCreateGameUI : public UUserWidget
{
    GENERATED_BODY()
protected:
    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    class UCCMenuButtonUI* B_CreateSession;

    UPROPERTY(meta = (BindWidget))
    class UCCMenuButtonUI* B_BackToMenu;

    UPROPERTY(meta = (BindWidget))
    class UCCMenuButtonUI* B_ApplyName;

    UPROPERTY(meta = (BindWidget))
    class UEditableTextBox* ET_PlayerName;

    UPROPERTY(meta = (BindWidget))
    class UEditableTextBox* ET_ServerName;

    UPROPERTY(meta = (BindWidget))
    class UEditableTextBox* ET_MaxPlayers;

    UPROPERTY(meta = (BindWidget))
    class UCheckBox* C_UseLan;

    UFUNCTION()
    void CreateSessionButtonClicked();

    UFUNCTION()
    void BackToMenuButtonlicked();

    UFUNCTION()
    void MaxPlayerNuberChanged(const FText& Text);

    void CreateSession();

    virtual void OnCreateSessionComplete(FName ServerName, bool Succeeded);

private:
    UPROPERTY()
    ACCHUDMainMenu* MainMenuHUD;

    IOnlineSessionPtr SessionInterface;
};
