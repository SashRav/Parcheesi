// Copyright Cats and Cubes. All Rights Reserved.


#include "UI/Widgets/CCPlayerTurnInfo.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UCCPlayerTurnInfo::SetPlayerInfoWidgetData(FString PlayerName, FLinearColor PlayerColor, bool TurnStatus) {
    T_PlayerName->SetText(FText::FromString(PlayerName.Right(5)));// For now will display the last 5 numbers of net ID
    I_PlayerColor->SetColorAndOpacity(PlayerColor);

    if (TurnStatus)
        T_TurnStatus->SetText(FText::FromString("Turn"));
    else
        T_TurnStatus->SetText(FText::FromString(""));
}       

