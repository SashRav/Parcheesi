// Copyright Cats and Cubes. All Rights Reserved.

#include "UI/CCHUDGame.h"
#include "UI/Widgets/CCGameLobbyUI.h"
#include "Framework/CCPlayerPawnGame.h"
#include "Framework/CCControllerGame.h"
#include "Kismet/GameplayStatics.h"
#include "Framework/CCGameModeBaseGame.h"
#include "GameFramework/PlayerController.h"

void ACCHUDGame::BeginPlay()
{
    check(LobbyWidgetClass);

    LobbyWidget = CreateWidget<UCCGameLobbyUI>(GetWorld(), LobbyWidgetClass);
    LobbyWidget->AddToViewport();
    LobbyWidget->OnStartGameButtonPressedEvent.AddDynamic(this, &ACCHUDGame::StartGameFromLobby);
    LobbyWidget->OnColorButtonPressed.AddDynamic(this, &ACCHUDGame::SelectColorInLobby);

    OwningPlayerPawn = Cast<ACCPlayerPawnGame>(GetOwningPawn());
}

void ACCHUDGame::RemoveLobbyWidget()
{
    if (LobbyWidget->IsInViewport())
    {
        LobbyWidget->RemoveFromParent();
        UE_LOG(LogTemp, Display, TEXT("Lobby widget removed"));
    }
    else
        UE_LOG(LogTemp, Display, TEXT("Lobby widget already removed"));
}

void ACCHUDGame::StartGameFromLobby()
{
    ACCGameModeBaseGame* ServerGameMode = Cast<ACCGameModeBaseGame>(UGameplayStatics::GetGameMode(GetWorld()));
    ServerGameMode->StartNewGame();
}

void ACCHUDGame::SelectColorInLobby(FName ColorTag)
{
    OwningPlayerPawn->Server_UpdateSelectedColor(ColorTag);
}
