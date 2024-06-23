// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CCMainMenuUI.generated.h"

class UButton;
class UCCMenuButtonUI;
class ACCHUDMainMenu;

UCLASS()
class PARCHEESI_API UCCMainMenuUI : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    class UCCMenuButtonUI* SingleplayerButton;

    UPROPERTY(meta = (BindWidget))
    class UCCMenuButtonUI* MultiplayerButton;

    UPROPERTY(meta = (BindWidget))
    class UCCMenuButtonUI* SettingsButton;

    UPROPERTY(meta = (BindWidget))
    class UCCMenuButtonUI* ExitButton;

    UFUNCTION()
    void SingleplayerButtonClicked();

    UFUNCTION()
    void MultiplayerButtonClicked();

    UFUNCTION()
    void SettingsButtonClicked();

    UFUNCTION()
    void ExitButtonClicked();

private:
    UPROPERTY()
    ACCHUDMainMenu* MainMenuHUD;
};
