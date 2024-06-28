// Copyright Cats and Cubes. All Rights Reserved.


#include "Framework/CCControllerMainMenu.h"

void ACCControllerMainMenu::BeginPlay() {

	SetInputMode(FInputModeUIOnly());
    bShowMouseCursor = true;
}
