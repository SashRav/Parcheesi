// Copyright Cats and Cubes. All Rights Reserved.

#include "UI/Widgets/CCGameLobbyUI.h"
#include "UI/Widgets/CCLobbyPlayerItem.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Slider.h"
#include "Components/CheckBox.h"
#include "Components/ScrollBox.h"
#include "CCCoreTypes.h"
#include "GameFramework/PlayerState.h"

void UCCGameLobbyUI::NativeConstruct()
{
    Super::NativeConstruct();

    B_StartGame->OnClicked.AddDynamic(this, &UCCGameLobbyUI::StartGameButtonClicked);
    B_SelectRed->OnClicked.AddDynamic(this, &UCCGameLobbyUI::SelectRedButtonClicked);
    B_SelectYellow->OnClicked.AddDynamic(this, &UCCGameLobbyUI::SelectYellowButtonClicked);
    B_SelectGreen->OnClicked.AddDynamic(this, &UCCGameLobbyUI::SelectGreenButtonClicked);
    B_SelectBlue->OnClicked.AddDynamic(this, &UCCGameLobbyUI::SelectBlueButtonClicked);
    B_ReadyToGame->OnClicked.AddDynamic(this, &UCCGameLobbyUI::ReadyButtonClicked);
    B_SaveSettings->OnClicked.AddDynamic(this, &UCCGameLobbyUI::SaveSettingsButtonClicked);
    B_ExitLobby->OnClicked.AddDynamic(this, &UCCGameLobbyUI::DisconnectCurrentPlayer);
    C_MoveFromStart->OnCheckStateChanged.AddDynamic(this, &UCCGameLobbyUI::MoveFromStartChecked);
    S_DicesCount->OnValueChanged.AddDynamic(this, &UCCGameLobbyUI::DicesToUseCountChanged);

    if (!GetOwningPlayer()->HasAuthority())
    {
        B_StartGame->SetVisibility(ESlateVisibility::Hidden);
        B_BotRed->SetVisibility(ESlateVisibility::Hidden);
        B_BotYellow->SetVisibility(ESlateVisibility::Hidden);
        B_BotGreen->SetVisibility(ESlateVisibility::Hidden);
        B_BotBlue->SetVisibility(ESlateVisibility::Hidden);
        C_MoveFromStart->SetIsEnabled(false);
        S_DicesCount->SetIsEnabled(false);
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

    B_SaveSettings->SetIsEnabled(false);
    B_SaveSettings->SetVisibility(ESlateVisibility::Hidden);

    T_PlayersReady->SetText(FText::FromString("Players Ready: 0/1"));

    B_ExitLobby->SetIsEnabled(true);

    /// Disable unused UI untill it will be implemented
    B_BotRed->SetIsEnabled(false);
    B_BotYellow->SetIsEnabled(false);
    B_BotGreen->SetIsEnabled(false);
    B_BotBlue->SetIsEnabled(false);
    B_BotRed->SetToolTipText(FText::FromString("Not yet implemented"));
    B_BotYellow->SetToolTipText(FText::FromString("Not yet implemented"));
    B_BotGreen->SetToolTipText(FText::FromString("Not yet implemented"));
    B_BotBlue->SetToolTipText(FText::FromString("Not yet implemented"));

    C_CatMode->SetIsEnabled(false);
    S_RoundsToBonus->SetIsEnabled(false);
    C_CatMode->SetToolTipText(FText::FromString("Not yet implemented"));
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

void UCCGameLobbyUI::SaveSettingsButtonClicked()
{
    if (GetOwningPlayer()->HasAuthority())
    {
        FGameSettings GameSettings;
        GameSettings.bMoveWithSix = C_MoveFromStart->IsChecked();
        GameSettings.DicesToRool = S_DicesCount->GetValue();

        OnSaveSettingsButtonPressed.Broadcast(GameSettings);

        B_SaveSettings->SetIsEnabled(false);
        B_SaveSettings->SetVisibility(ESlateVisibility::Hidden);
    }
}

void UCCGameLobbyUI::DicesToUseCountChanged(float Value)
{
    if (GetOwningPlayer()->HasAuthority())
    {
        T_DiceCount->SetText(FText::FromString(FString::SanitizeFloat(Value, 0)));
        B_SaveSettings->SetIsEnabled(true);
        B_SaveSettings->SetVisibility(ESlateVisibility::Visible);
    }
}
void UCCGameLobbyUI::MoveFromStartChecked(bool bIsChecked)
{
    if (GetOwningPlayer()->HasAuthority())
    {
        B_SaveSettings->SetIsEnabled(true);
        B_SaveSettings->SetVisibility(ESlateVisibility::Visible);
    }
}

void UCCGameLobbyUI::UpdateSettings(FGameSettings GameSettings)
{
    T_DiceCount->SetText(FText::FromString(FString::SanitizeFloat(GameSettings.DicesToRool, 0)));
    S_DicesCount->SetValue(GameSettings.DicesToRool);
    C_MoveFromStart->SetIsChecked(GameSettings.bMoveWithSix);
}

void UCCGameLobbyUI::UpdatePlayersList(const TArray<FUniqueNetIdRepl>& AllPlayers)
{
    for (int32 i = 0; i < SB_PlayersList->GetChildrenCount(); i++)
    {
        UCCLobbyPlayerItem* ChildWidget = Cast<UCCLobbyPlayerItem>(SB_PlayersList->GetChildAt(i));
        if (ChildWidget)
        {
            ChildWidget->OnKickPlayerButtonPressed.RemoveAll(this);
        }
    }

    SB_PlayersList->ClearChildren();

    if (AllPlayers.Num() == 0)
        return;

    for (FUniqueNetIdRepl Item : AllPlayers)
    {
        UCCLobbyPlayerItem* PlayerInfo = CreateWidget<UCCLobbyPlayerItem>(GetWorld(), LobbyPlayerItemClass);
        bool bShowButton = GetOwningPlayer()->HasAuthority();

        if (bShowButton && GetOwningPlayerPawn()->GetPlayerState()->GetUniqueId() == Item)
            bShowButton = false;

        PlayerInfo->SetPlayerData(FText::FromString(Item->ToString().Left(20)), Item, bShowButton); // Hardcoded untill using Players names

        if (bShowButton)
            PlayerInfo->OnKickPlayerButtonPressed.AddDynamic(this,
                &UCCGameLobbyUI::DisconnectPlayerByID); // Potential memory leak. Will need to check does GC removes widgets with binidng

        SB_PlayersList->AddChild(PlayerInfo);
    }
}

void UCCGameLobbyUI::DisconnectCurrentPlayer()
{
    OnExitToMenuButtonPressed.Broadcast();
}

void UCCGameLobbyUI::DisconnectPlayerByID(FUniqueNetIdRepl PlayerNetID)
{
    OnKickPlayerFromLobby.Broadcast(PlayerNetID);
}
