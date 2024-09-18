// Copyright Cats and Cubes. All Rights Reserved.

#include "UI/Widgets/CCLobbyPlayerItem.h"
#include "Components/TextBlock.h"

void UCCLobbyPlayerItem::SetPlayerName(FText Name)
{
    T_PlayerName->SetText(Name);
}
