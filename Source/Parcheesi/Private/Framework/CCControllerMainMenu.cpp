// Copyright Cats and Cubes. All Rights Reserved.


#include "Framework/CCControllerMainMenu.h"

void ACCControllerMainMenu::BeginPlay() {

	this->SetInputMode(FInputModeUIOnly());
    this->bShowMouseCursor = true;
}
