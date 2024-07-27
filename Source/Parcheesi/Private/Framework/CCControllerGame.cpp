// Copyright Cats and Cubes. All Rights Reserved.

#include "Framework/CCControllerGame.h"
#include "Framework/CCPlayerStateGame.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"

void ACCControllerGame::BeginPlay()
{
    SetInputMode(FInputModeUIOnly());
    bShowMouseCursor = true;

    PlayerStateLocal = Cast<ACCPlayerStateGame>(PlayerState);
}
