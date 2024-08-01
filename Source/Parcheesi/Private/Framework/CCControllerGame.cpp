// Copyright Cats and Cubes. All Rights Reserved.

#include "Framework/CCControllerGame.h"
#include "UI/CCHUDGame.h"

void ACCControllerGame::Client_StartGameFromController_Implementation()
{
    if (OwningHUD)
        OwningHUD->RemoveLobbyWidget();
}

void ACCControllerGame::BeginPlay()
{
    SetInputMode(FInputModeUIOnly());
    bShowMouseCursor = true;

    OwningHUD = Cast<ACCHUDGame>(GetHUD());
}
