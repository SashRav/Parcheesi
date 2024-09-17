// Copyright Cats and Cubes. All Rights Reserved.

#include "UI/Widgets/CCGameLobbyUI.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Slider.h"
#include "Components/CheckBox.h"
#include "CCCoreTypes.h"

void UCCGameLobbyUI::NativeConstruct()
{
    B_StartGame->OnClicked.AddDynamic(this, &UCCGameLobbyUI::StartGameButtonClicked);
    B_SelectRed->OnClicked.AddDynamic(this, &UCCGameLobbyUI::SelectRedButtonClicked);
    B_SelectYellow->OnClicked.AddDynamic(this, &UCCGameLobbyUI::SelectYellowButtonClicked);
    B_SelectGreen->OnClicked.AddDynamic(this, &UCCGameLobbyUI::SelectGreenButtonClicked);
    B_SelectBlue->OnClicked.AddDynamic(this, &UCCGameLobbyUI::SelectBlueButtonClicked);
    B_ReadyToGame->OnClicked.AddDynamic(this, &UCCGameLobbyUI::ReadyButtonClicked);

    if (!GetOwningPlayer()->HasAuthority())
    {
        B_StartGame->SetVisibility(ESlateVisibility::Hidden);
        B_BotRed->SetVisibility(ESlateVisibility::Hidden);
        B_BotYellow->SetVisibility(ESlateVisibility::Hidden);
        B_BotGreen->SetVisibility(ESlateVisibility::Hidden);
        B_BotBlue->SetVisibility(ESlateVisibility::Hidden);
    }
    else
    {
        B_ReadyToGame->SetVisibility(ESlateVisibility::Hidden);
    }

    T_RedPlayerName->SetText(FText::FromString("Empty"));
    T_YellowPlayerName->SetText(FText::FromString("Empty"));
    T_GreenPlayerName->SetText(FText::FromString("Empty"));
    T_BluePlayerName->SetText(FText::FromString("Empty"));

    B_StartGame->SetIsEnabled(false);
    B_ReadyToGame->SetIsEnabled(false);
    B_StartGame->SetToolTipText(FText::FromString("You need at least 2 players ready to start the game"));
    
    T_PlayersReady->SetText(FText::FromString("Players Ready: 0/1"));

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

void UCCGameLobbyUI::ReadyButtonClicked()
{
    if (!GetOwningPlayer()->HasAuthority())
    {
        B_ReadyToGame->SetIsEnabled(false);
    }

    OnReadyButtonPressed.Broadcast();
}

void UCCGameLobbyUI::SelectRedButtonClicked()
{
    if (!GetOwningPlayer()->HasAuthority())
    {
        B_ReadyToGame->SetIsEnabled(true);
    }
    OnColorButtonPressed.Broadcast("Red");
}

void UCCGameLobbyUI::SelectYellowButtonClicked()
{
    if (!GetOwningPlayer()->HasAuthority())
    {
        B_ReadyToGame->SetIsEnabled(true);
    }
    OnColorButtonPressed.Broadcast("Yellow");
}

void UCCGameLobbyUI::SelectGreenButtonClicked()
{
    if (!GetOwningPlayer()->HasAuthority())
    {
        B_ReadyToGame->SetIsEnabled(true);
    }
    OnColorButtonPressed.Broadcast("Green");
}

void UCCGameLobbyUI::SelectBlueButtonClicked()
{
    if (!GetOwningPlayer()->HasAuthority())
    {
        B_ReadyToGame->SetIsEnabled(true);
    }
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

    T_RedPlayerReady->SetText(FText::FromString("Not Ready"));
    T_YellowPlayerReady->SetText(FText::FromString("Not Ready"));
    T_GreenPlayerReady->SetText(FText::FromString("Not Ready"));
    T_BluePlayerReady->SetText(FText::FromString("Not Ready"));

    B_SelectRed->SetIsEnabled(true);
    B_SelectYellow->SetIsEnabled(true);
    B_SelectGreen->SetIsEnabled(true);
    B_SelectBlue->SetIsEnabled(true);

    for (FAllPlayersData Player : AllPlayersData)
    {
        FText IsReadyText = FText::FromString("Not Ready");
        if (Player.bIsReady)
            IsReadyText = FText::FromString("Ready");

        UpdateSelectionPlayerName(Player.PlayerName, Player.Tag, IsReadyText);
    }
    UpdateReadyPlayers(AllPlayersData);
}

void UCCGameLobbyUI::UpdateSelectionPlayerName(FText PlayerName, FName Tag, FText IsReadyText)
{
    if (Tag == "Red")
    {
        T_RedPlayerName->SetText(PlayerName);
        B_SelectRed->SetIsEnabled(false);
        T_RedPlayerReady->SetText(IsReadyText);
        return;
    }
    if (Tag == "Yellow")
    {
        T_YellowPlayerName->SetText(PlayerName);
        B_SelectYellow->SetIsEnabled(false);
        T_YellowPlayerReady->SetText(IsReadyText);
        return;
    }
    if (Tag == "Green")
    {
        T_GreenPlayerName->SetText(PlayerName);
        B_SelectGreen->SetIsEnabled(false);
        T_GreenPlayerReady->SetText(IsReadyText);
        return;
    }
    if (Tag == "Blue")
    {
        T_BluePlayerName->SetText(PlayerName);
        B_SelectBlue->SetIsEnabled(false);
        T_BluePlayerReady->SetText(IsReadyText);
        return;
    }
}

void UCCGameLobbyUI::UpdateReadyPlayers(const TArray<FAllPlayersData>& AllPlayersData)
{
    int32 TotalPlayers = AllPlayersData.Num();

    if (TotalPlayers == 0)
        return;

    int32 ReadyPlayers = 0;

    for (FAllPlayersData PlayerData : AllPlayersData)
    {
        if (PlayerData.bIsReady)
            ReadyPlayers++;
    }

    FString ResultString = "Players Ready: " + FString::FromInt(ReadyPlayers) + "/" + FString::FromInt(TotalPlayers);
    T_PlayersReady->SetText(FText::FromString(ResultString));

    if (!GetOwningPlayer()->HasAuthority())
        return;

    if (TotalPlayers > 1 && ReadyPlayers == TotalPlayers)
    {
        B_StartGame->SetIsEnabled(true);
    }
}
