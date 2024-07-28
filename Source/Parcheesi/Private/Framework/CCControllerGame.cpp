// Copyright Cats and Cubes. All Rights Reserved.

#include "Framework/CCControllerGame.h"

void ACCControllerGame::BeginPlay()
{
    SetInputMode(FInputModeUIOnly());
    bShowMouseCursor = true;
}
