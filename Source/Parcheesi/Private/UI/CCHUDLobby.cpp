// Copyright Cats and Cubes. All Rights Reserved.

#include "UI/CCHUDLobby.h"
#include "UI/Widgets/CCGameLobbyUI.h"
#include "Framework/CCGameModeBaseGame.h"
#include "Kismet/GameplayStatics.h"

void ACCHUDLobby::BeginPlay()
{
    check(LobbyWidgetClass);

    LobbyWidget = CreateWidget<UCCGameLobbyUI>(GetWorld(), LobbyWidgetClass);
    LobbyWidget->AddToViewport();
    LobbyWidget->OnStartGameButtonPressedEvent.AddDynamic(this, &ACCHUDLobby::StartGameFromLobby);
}

void ACCHUDLobby::StartGameFromLobby()
{
    ACCGameModeBaseGame* GameMode = Cast<ACCGameModeBaseGame>(UGameplayStatics::GetGameMode(GetWorld()));
    if (GameMode)
    {
        GameMode->StartNewGame();
        LobbyWidget->RemoveFromViewport();
    }
}
