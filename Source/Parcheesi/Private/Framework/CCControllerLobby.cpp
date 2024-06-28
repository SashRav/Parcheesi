// Copyright Cats and Cubes. All Rights Reserved.

#include "Framework/CCControllerLobby.h"

void ACCControllerLobby::BeginPlay()
{
    SetInputMode(FInputModeUIOnly());
    bShowMouseCursor = true;
}
