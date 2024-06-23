// Copyright Cats and Cubes. All Rights Reserved.

#include "UI/CCHUDMainMenu.h"
#include "Blueprint/UserWidget.h"


void ACCHUDMainMenu::BeginPlay()
{
    check(MainMenuWidgetClass);
    check(SingleplayerWidgetClass);
    check(MultiplayerWidgetClass);
    check(SettingsWidgetClass);
    check(CreateGameWidgetClass);
    check(SearchGameWidgetClass);

    MainMenuWidget = CreateWidget<UUserWidget>(GetWorld(), MainMenuWidgetClass);
    SingleplayerWidget = CreateWidget<UUserWidget>(GetWorld(), SingleplayerWidgetClass);
    MultiplayerWidget = CreateWidget<UUserWidget>(GetWorld(), MultiplayerWidgetClass);
    SettingsWidget = CreateWidget<UUserWidget>(GetWorld(), SettingsWidgetClass);
    CreateGameWidget = CreateWidget<UUserWidget>(GetWorld(), CreateGameWidgetClass);
    SearchGameWidget = CreateWidget<UUserWidget>(GetWorld(), SearchGameWidgetClass);
    ShowMainMenuWidget();
}

void ACCHUDMainMenu::HideAllWidgets()
{
    MainMenuWidget->RemoveFromViewport();
    SingleplayerWidget->RemoveFromViewport();
    MultiplayerWidget->RemoveFromViewport();
    SettingsWidget->RemoveFromViewport();
    CreateGameWidget->RemoveFromViewport();
    SearchGameWidget->RemoveFromViewport();
}

void ACCHUDMainMenu::ShowMainMenuWidget()
{
    HideAllWidgets();
    MainMenuWidget->AddToViewport();
}

void ACCHUDMainMenu::ShowSingleplayerWidget()
{
    HideAllWidgets();
    SingleplayerWidget->AddToViewport();
}

void ACCHUDMainMenu::ShowMultiplayerWidget()
{
    HideAllWidgets();
    MultiplayerWidget->AddToViewport();
}

void ACCHUDMainMenu::ShowSettingsWidget()
{
    HideAllWidgets();
    SettingsWidget->AddToViewport();
}

void ACCHUDMainMenu::ShowCreateGameWidget()
{
    HideAllWidgets();
    CreateGameWidget->AddToViewport();
}

void ACCHUDMainMenu::ShowJoinGameWidget()
{
    HideAllWidgets();
    SearchGameWidget->AddToViewport();
}
