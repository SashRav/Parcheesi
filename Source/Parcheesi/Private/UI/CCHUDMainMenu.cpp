// Copyright Cats and Cubes. All Rights Reserved.

#include "UI/CCHUDMainMenu.h"
#include "Blueprint/UserWidget.h"

void ACCHUDMainMenu::BeginPlay()
{
    MainMenuWidget = CreateWidget<UUserWidget>(GetWorld(), MainMenuWidgetClass);
    MainMenuWidget->AddToViewport();
}
