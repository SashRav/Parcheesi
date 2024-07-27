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

    ServerGameMode = Cast<ACCGameModeBaseGame>(UGameplayStatics::GetGameMode(GetWorld()));

    //if (ServerGameMode)
    //{
    //    ServerGameMode->OnGameStartedEvent.AddDynamic(this, &ACCHUDGame::RemoveLobbyWidget);
    //}
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
    if (ServerGameMode)
    {
        ServerGameMode->StartNewGame();
    }
}


