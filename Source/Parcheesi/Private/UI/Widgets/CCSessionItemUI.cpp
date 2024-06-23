// Copyright Cats and Cubes. All Rights Reserved.


#include "UI/Widgets/CCSessionItemUI.h"
#include "Components/TextBlock.h"

void UCCSessionItemUI::SetSessionWidgetData(FText ServerName, int Players, int Ping) {
    ServerNameText->SetText(ServerName);
    PlayersText->SetText(FText::AsNumber(Players));
    PingText->SetText(FText::AsNumber(Ping));
}
