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
    MainMenuWidget->AddToViewport();
    SingleplayerWidget->AddToViewport();
    MultiplayerWidget->AddToViewport();
    SettingsWidget->AddToViewport();
    CreateGameWidget->AddToViewport();
    SearchGameWidget->AddToViewport();

    ShowMainMenuWidget();
}

void ACCHUDMainMenu::HideAllWidgets()
{
    MainMenuWidget->SetVisibility(ESlateVisibility::Hidden);
    SingleplayerWidget->SetVisibility(ESlateVisibility::Hidden);
    MultiplayerWidget->SetVisibility(ESlateVisibility::Hidden);
    SettingsWidget->SetVisibility(ESlateVisibility::Hidden);
    CreateGameWidget->SetVisibility(ESlateVisibility::Hidden);
    SearchGameWidget->SetVisibility(ESlateVisibility::Hidden);
}

void ACCHUDMainMenu::ShowMainMenuWidget()
{
    HideAllWidgets();
    MainMenuWidget->SetVisibility(ESlateVisibility::Visible);
}

void ACCHUDMainMenu::ShowSingleplayerWidget()
{
    HideAllWidgets();
    SingleplayerWidget->SetVisibility(ESlateVisibility::Visible);
}

void ACCHUDMainMenu::ShowMultiplayerWidget()
{
    HideAllWidgets();
    MultiplayerWidget->SetVisibility(ESlateVisibility::Visible);
}

void ACCHUDMainMenu::ShowSettingsWidget()
{
    HideAllWidgets();
    SettingsWidget->SetVisibility(ESlateVisibility::Visible);
}

void ACCHUDMainMenu::ShowCreateGameWidget()
{
    HideAllWidgets();
    CreateGameWidget->SetVisibility(ESlateVisibility::Visible);
}

void ACCHUDMainMenu::ShowJoinGameWidget()
{
    HideAllWidgets();
    SearchGameWidget->SetVisibility(ESlateVisibility::Visible);
}
