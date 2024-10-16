// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CCMenuSingleplayerUI.generated.h"

class UCCMenuButtonUI;
class ACCHUDMainMenu;

UCLASS()
class PARCHEESI_API UCCMenuSingleplayerUI : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    class UCCMenuButtonUI* B_StartGame;

    UPROPERTY(meta = (BindWidget))
    class UCCMenuButtonUI* B_BackToMenu;

    UFUNCTION()
    void StartGameButtonClicked();

    UFUNCTION()
    void BackToMenuButtonlicked();

    void StartGame();

private:
    UPROPERTY()
    ACCHUDMainMenu* MainMenuHUD;
};
