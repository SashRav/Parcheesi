// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CCHUDMainMenu.generated.h"

UCLASS()
class PARCHEESI_API ACCHUDMainMenu : public AHUD
{
    GENERATED_BODY()

public:
    void ShowSingleplayerWidget();
    void ShowMultiplayerWidget();
    void ShowSettingsWidget();
    void ShowMainMenuWidget();

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> MainMenuWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> SingleplayerWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> MultiplayerWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> SettingsWidgetClass;

    virtual void BeginPlay() override;

private:
    UPROPERTY()
    UUserWidget* MainMenuWidget = nullptr;

    UPROPERTY()
    UUserWidget* SingleplayerWidget = nullptr;

    UPROPERTY()
    UUserWidget* MultiplayerWidget = nullptr;

    UPROPERTY()
    UUserWidget* SettingsWidget = nullptr;
};
