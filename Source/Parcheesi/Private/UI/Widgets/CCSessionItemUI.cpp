// Copyright Cats and Cubes. All Rights Reserved.


#include "UI/Widgets/CCSessionItemUI.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "OnlineSessionSettings.h"



FReply UCCSessionItemUI::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

    OnSessionItemPressedEvent.Broadcast(LocalSessionData);

    return FReply::Handled();
    
}

void UCCSessionItemUI::SetSessionWidgetData(FString ServerName, int Players, int Ping)
{
    ServerNameText->SetText(FText::FromString(ServerName));
    PlayersText->SetText(FText::AsNumber(Players));
    PingText->SetText(FText::AsNumber(Ping));
}

void UCCSessionItemUI::SetSessionData(FOnlineSessionSearchResult SessionData) {
    LocalSessionData = SessionData;
    SetSessionWidgetData(SessionData.GetSessionIdStr(), 1, SessionData.PingInMs);
}

