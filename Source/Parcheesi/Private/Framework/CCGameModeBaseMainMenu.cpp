// Copyright Cats and Cubes. All Rights Reserved.

#include "Framework/CCGameModeBaseMainMenu.h"
#include "Framework/CCControllerMainMenu.h"
#include "UI/CCHUDMainMenu.h"

ACCGameModeBaseMainMenu::ACCGameModeBaseMainMenu()
{
    PlayerControllerClass = ACCControllerMainMenu::StaticClass();
    HUDClass = ACCHUDMainMenu::StaticClass();
}
