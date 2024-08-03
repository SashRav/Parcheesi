// Copyright Cats and Cubes. All Rights Reserved.

#include "Framework/CCControllerGame.h"
#include "UI/CCHUDGame.h"
#include "CCCoreTypes.h"

void ACCControllerGame::Client_StartGameFromController_Implementation()
{
    if (OwningHUD)
        OwningHUD->RemoveLobbyWidget();
}

void ACCControllerGame::Client_UpdateTurnWidgets_Implementation(const TArray<FPlayersTurnData>& PlayersTurnData) {
    if (OwningHUD)
        OwningHUD->UpdateTurnWidgets(PlayersTurnData);
}

void ACCControllerGame::BeginPlay()
{
    SetInputMode(FInputModeUIOnly());
    bShowMouseCursor = true;

    OwningHUD = Cast<ACCHUDGame>(GetHUD());
}
