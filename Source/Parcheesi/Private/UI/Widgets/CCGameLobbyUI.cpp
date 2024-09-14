// Copyright Cats and Cubes. All Rights Reserved.

#include "UI/Widgets/CCGameLobbyUI.h"
#include "Components/Button.h"
#include "GameFramework/PlayerController.h"

void UCCGameLobbyUI::NativeConstruct()
{
    B_StartGame->OnClicked.AddDynamic(this, &UCCGameLobbyUI::StartGameButtonClicked);
    B_SelectRed->OnClicked.AddDynamic(this, &UCCGameLobbyUI::SelectRedButtonClicked);
    B_SelectYellow->OnClicked.AddDynamic(this, &UCCGameLobbyUI::SelectYellowButtonClicked);
    B_SelectGreen->OnClicked.AddDynamic(this, &UCCGameLobbyUI::SelectGreenButtonClicked);
    B_SelectBlue->OnClicked.AddDynamic(this, &UCCGameLobbyUI::SelectBlueButtonClicked);

    if (!GetOwningPlayer()->HasAuthority())
    {
        B_StartGame->SetIsEnabled(false);
        B_BotRed->SetIsEnabled(false);
        B_BotRed->SetVisibility(ESlateVisibility::Hidden);
        B_BotYellow->SetIsEnabled(false);
        B_BotYellow->SetVisibility(ESlateVisibility::Hidden);
        B_BotGreen->SetIsEnabled(false);
        B_BotGreen->SetVisibility(ESlateVisibility::Hidden);
        B_BotBlue->SetIsEnabled(false);
        B_BotBlue->SetVisibility(ESlateVisibility::Hidden);
    }
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
