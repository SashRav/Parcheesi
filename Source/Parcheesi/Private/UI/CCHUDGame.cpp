// Copyright Cats and Cubes. All Rights Reserved.


#include "UI/CCHUDGame.h"
#include "UI/Widgets/CCGameLobbyUI.h"
#include "Framework/CCPlayerPawnGame.h"

void ACCHUDGame::BeginPlay()
{
    check(LobbyWidgetClass);

    LobbyWidget = CreateWidget<UCCGameLobbyUI>(GetWorld(), LobbyWidgetClass);
    LobbyWidget->AddToViewport();
    LobbyWidget->OnStartGameButtonPressedEvent.AddDynamic(this, &ACCHUDGame::StartGameFromLobby);
    LobbyWidget->OnColorButtonPressed.AddDynamic(this, &ACCHUDGame::SelectColorInLobby);

    OwningPlayerPawn = Cast<ACCPlayerPawnGame>(GetOwningPawn());
}

void ACCHUDGame::RemoveLobbyWidget() {
    if (LobbyWidget->IsInViewport())
    {
        LobbyWidget->RemoveFromParent();
        UE_LOG(LogTemp, Display, TEXT("Lobby widget removed"));
    }
    else
        UE_LOG(LogTemp, Display, TEXT("Lobby widget removed"));
}

void ACCHUDGame::StartGameFromLobby() {
    
}

void ACCHUDGame::SelectColorInLobby(FName ColorTag)
{
    OwningPlayerPawn->Server_UpdateSelectedColor(ColorTag);
}


