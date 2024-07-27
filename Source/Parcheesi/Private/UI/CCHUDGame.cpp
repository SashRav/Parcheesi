// Copyright Cats and Cubes. All Rights Reserved.


#include "UI/CCHUDGame.h"
#include "UI/Widgets/CCGameLobbyUI.h"
#include "Framework/CCGameModeBaseGame.h"
#include "Kismet/GameplayStatics.h"

void ACCHUDGame::BeginPlay()
{
    check(LobbyWidgetClass);

    LobbyWidget = CreateWidget<UCCGameLobbyUI>(GetWorld(), LobbyWidgetClass);
    LobbyWidget->AddToViewport();
    LobbyWidget->OnStartGameButtonPressedEvent.AddDynamic(this, &ACCHUDGame::StartGameFromLobby);
}

void ACCHUDGame::StartGameFromLobby() {
    ACCGameModeBaseGame* GameMode = Cast<ACCGameModeBaseGame>(UGameplayStatics::GetGameMode(GetWorld()));
    if (GameMode)
    {
        GameMode->StartNewGame();
        LobbyWidget->RemoveFromViewport();
    }
}


