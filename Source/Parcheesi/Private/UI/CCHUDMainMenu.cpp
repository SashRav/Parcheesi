// Copyright Cats and Cubes. All Rights Reserved.

#include "UI/CCHUDMainMenu.h"
#include "Blueprint/UserWidget.h"

void ACCHUDMainMenu::BeginPlay()
{
    check(MainMenuWidgetClass);
    check(SingleplayerWidgetClass);
    check(MultiplayerWidgetClass);
    check(SettingsWidgetClass);

    MainMenuWidget = CreateWidget<UUserWidget>(GetWorld(), MainMenuWidgetClass);
    SingleplayerWidget = CreateWidget<UUserWidget>(GetWorld(), SingleplayerWidgetClass);
    MultiplayerWidget = CreateWidget<UUserWidget>(GetWorld(), MultiplayerWidgetClass);
    SettingsWidget = CreateWidget<UUserWidget>(GetWorld(), SettingsWidgetClass);
    ShowMainMenuWidget();
}

void ACCHUDMainMenu::ShowMainMenuWidget() {
    MainMenuWidget->AddToViewport();
    SingleplayerWidget->RemoveFromViewport();
    MultiplayerWidget->RemoveFromViewport();
    SettingsWidget->RemoveFromViewport();
}

void ACCHUDMainMenu::ShowSingleplayerWidget() {
    MainMenuWidget->RemoveFromViewport();
    SingleplayerWidget->AddToViewport();
    MultiplayerWidget->RemoveFromViewport();
    SettingsWidget->RemoveFromViewport();
}

void ACCHUDMainMenu::ShowMultiplayerWidget() {
    MainMenuWidget->RemoveFromViewport();
    SingleplayerWidget->RemoveFromViewport();
    MultiplayerWidget->AddToViewport();
    SettingsWidget->RemoveFromViewport();
}

void ACCHUDMainMenu::ShowSettingsWidget() {
    MainMenuWidget->RemoveFromViewport();
    SingleplayerWidget->RemoveFromViewport();
    MultiplayerWidget->RemoveFromViewport();
    SettingsWidget->AddToViewport();
}
