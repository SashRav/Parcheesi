// Copyright Cats and Cubes. All Rights Reserved.

#include "UI/Widgets/CCGameLobbyUI.h"
#include "Components/Button.h"
#include "GameFramework/PlayerController.h"

void UCCGameLobbyUI::NativeConstruct()
{
    StartGameButton->OnClicked.AddDynamic(this, &UCCGameLobbyUI::StartGameButtonClicked);
    SelectRedButton->OnClicked.AddDynamic(this, &UCCGameLobbyUI::SelectRedButtonClicked);
    SelectYellowButton->OnClicked.AddDynamic(this, &UCCGameLobbyUI::SelectYellowButtonClicked);
    SelectGreenButton->OnClicked.AddDynamic(this, &UCCGameLobbyUI::SelectGreenButtonClicked);
    SelectBlueButton->OnClicked.AddDynamic(this, &UCCGameLobbyUI::SelectBlueButtonClicked);

    if (!GetOwningPlayer()->HasAuthority())
        StartGameButton->SetIsEnabled(false);
}

void UCCGameLobbyUI::StartGameButtonClicked()
{
    OnStartGameButtonPressedEvent.Broadcast();
}

void UCCGameLobbyUI::SelectRedButtonClicked() {
    OnColorButtonPressed.Broadcast("Red");
}

void UCCGameLobbyUI::SelectYellowButtonClicked() {
    OnColorButtonPressed.Broadcast("Yellow");
}

void UCCGameLobbyUI::SelectGreenButtonClicked() {
    OnColorButtonPressed.Broadcast("Green");
}

void UCCGameLobbyUI::SelectBlueButtonClicked() {
    OnColorButtonPressed.Broadcast("Blue");
}
