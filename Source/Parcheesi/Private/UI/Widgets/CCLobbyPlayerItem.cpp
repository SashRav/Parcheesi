// Copyright Cats and Cubes. All Rights Reserved.

#include "UI/Widgets/CCLobbyPlayerItem.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "CCCoreTypes.h"

void UCCLobbyPlayerItem::NativeConstruct() {
    B_KickPlayer->OnClicked.AddDynamic(this, &UCCLobbyPlayerItem::KickPlayer);
}

void UCCLobbyPlayerItem::KickPlayer() 
{
    OnKickPlayerButtonPressed.Broadcast(PlayerNetID);
}

void UCCLobbyPlayerItem::SetPlayerData(FLobbyPlayersData PlayersData, bool bEnableButton)
{
    if (!bEnableButton)
    {
        B_KickPlayer->SetIsEnabled(false);
        B_KickPlayer->SetVisibility(ESlateVisibility::Hidden);
    }
    T_PlayerName->SetText(PlayersData.PlayerName);
    PlayerNetID = PlayersData.NetId;
}

