// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CCMultiplayerUI.generated.h"

class UCCMenuButtonUI;
class ACCHUDMainMenu;

UCLASS()
class PARCHEESI_API UCCMultiplayerUI : public UUserWidget
{
    GENERATED_BODY()
protected:
    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    class UCCMenuButtonUI* CreateGameButton;

    UPROPERTY(meta = (BindWidget))
    class UCCMenuButtonUI* JoinGameButton;

    UPROPERTY(meta = (BindWidget))
    class UCCMenuButtonUI* BackToMenuButton;

    UFUNCTION()
    void CreateGameButtonClicked();

    UFUNCTION()
    void JoinGameButtonClicked();

    UFUNCTION()
    void BackToMenuButtonlicked();

private:
    UPROPERTY()
    ACCHUDMainMenu* MainMenuHUD;
};
