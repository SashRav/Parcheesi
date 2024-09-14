// Copyright Cats and Cubes. All Rights Reserved.

#include "UI/Widgets/CCGameLobbyUI.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Slider.h"
#include "Components/CheckBox.h"
#include "GameFramework/PlayerController.h"
#include "CCCoreTypes.h"

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

    T_RedPlayerName->SetText(FText::FromString("Empty"));
    T_YellowPlayerName->SetText(FText::FromString("Empty"));
    T_GreenPlayerName->SetText(FText::FromString("Empty"));
    T_BluePlayerName->SetText(FText::FromString("Empty"));

    /// Disable unused UI untill it will be implemented
    B_BotRed->SetIsEnabled(false);
    B_BotYellow->SetIsEnabled(false);
    B_BotGreen->SetIsEnabled(false);
    B_BotBlue->SetIsEnabled(false);
    B_BotRed->SetToolTipText(FText::FromString("Not yet implemented"));
    B_BotYellow->SetToolTipText(FText::FromString("Not yet implemented"));
    B_BotGreen->SetToolTipText(FText::FromString("Not yet implemented"));
    B_BotBlue->SetToolTipText(FText::FromString("Not yet implemented"));

    B_ExitLobby->SetIsEnabled(false);
    B_SaveSettings->SetIsEnabled(false);
    B_ExitLobby->SetToolTipText(FText::FromString("Not yet implemented"));
    B_SaveSettings->SetToolTipText(FText::FromString("Not yet implemented"));

    C_CatMode->SetIsEnabled(false);
    C_MoveFromStart->SetIsEnabled(false);
    S_DicesCount->SetIsEnabled(false);
    S_RoundsToBonus->SetIsEnabled(false);
    C_CatMode->SetToolTipText(FText::FromString("Not yet implemented"));
    C_MoveFromStart->SetToolTipText(FText::FromString("Not yet implemented"));
    S_DicesCount->SetToolTipText(FText::FromString("Not yet implemented"));
    S_RoundsToBonus->SetToolTipText(FText::FromString("Not yet implemented"));
}

void UCCGameLobbyUI::StartGameButtonClicked()
{
    OnStartGameButtonPressedEvent.Broadcast();
}

void UCCGameLobbyUI::SelectRedButtonClicked()
{
    OnColorButtonPressed.Broadcast("Red");
}

void UCCGameLobbyUI::SelectYellowButtonClicked()
{
    OnColorButtonPressed.Broadcast("Yellow");
}

void UCCGameLobbyUI::SelectGreenButtonClicked()
{
    OnColorButtonPressed.Broadcast("Green");
}

void UCCGameLobbyUI::SelectBlueButtonClicked()
{
    OnColorButtonPressed.Broadcast("Blue");
}

void UCCGameLobbyUI::UpdateSelectionStatus(const TArray<FAllPlayersData>& AllPlayersData)
{
    if (AllPlayersData.Num() == 0)
        return;

    T_RedPlayerName->SetText(FText::FromString("Empty"));
    T_YellowPlayerName->SetText(FText::FromString("Empty"));
    T_GreenPlayerName->SetText(FText::FromString("Empty"));
    T_BluePlayerName->SetText(FText::FromString("Empty"));
    B_SelectRed->SetIsEnabled(true);
    B_SelectYellow->SetIsEnabled(true);
    B_SelectGreen->SetIsEnabled(true);
    B_SelectBlue->SetIsEnabled(true);

    for (FAllPlayersData Player : AllPlayersData)
    {
        UpdateSelectionPlayerName(Player.PlayerName, Player.Tag);
    }
}

void UCCGameLobbyUI::UpdateSelectionPlayerName(FText PlayerName, FName Tag)
{
    if (Tag == "Red")
    {
        T_RedPlayerName->SetText(PlayerName);
        B_SelectRed->SetIsEnabled(false);
        return;
    }
    if (Tag == "Yellow")
    {
        T_YellowPlayerName->SetText(PlayerName);
        B_SelectYellow->SetIsEnabled(false);
        return;
    }
    if (Tag == "Green")
    {
        T_GreenPlayerName->SetText(PlayerName);
        B_SelectGreen->SetIsEnabled(false);
        return;
    }
    if (Tag == "Blue")
    {
        T_BluePlayerName->SetText(PlayerName);
        B_SelectBlue->SetIsEnabled(false);
        return;
    }
}