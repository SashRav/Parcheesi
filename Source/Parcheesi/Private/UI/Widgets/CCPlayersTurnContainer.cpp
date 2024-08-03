// Copyright Cats and Cubes. All Rights Reserved.

#include "UI/Widgets/CCPlayersTurnContainer.h"
#include "UI/Widgets/CCPlayerTurnInfo.h"
#include "Components/VerticalBox.h"
#include "CCCoreTypes.h"

void UCCPlayersTurnContainer::UpdatePlayersTurnData(const TArray<FPlayersTurnData>& PlayersTurnData)
{
    VB_Players->ClearChildren();

    for (FPlayersTurnData PlayerData : PlayersTurnData)
    {
        UCCPlayerTurnInfo* PlayerTurnWidget = CreateWidget<UCCPlayerTurnInfo>(GetWorld(), TurnInfoWidgetClass);
        PlayerTurnWidget->SetPlayerInfoWidgetData(PlayerData.PlayerName, PlayerData.PlayerColor, PlayerData.TurnSatus);
        VB_Players->AddChild(PlayerTurnWidget);
    }
}
