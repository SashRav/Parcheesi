// Copyright Cats and Cubes. All Rights Reserved.

#include "UI/Widgets/CCGameLobbyUI.h"
#include "Components/Button.h"

void UCCGameLobbyUI::NativeConstruct()
{
    StartGameButton->OnClicked.AddDynamic(this, &UCCGameLobbyUI::StartGameButtonClicked);
}

void UCCGameLobbyUI::StartGameButtonClicked()
{
    OnStartGameButtonPressedEvent.Broadcast();
}