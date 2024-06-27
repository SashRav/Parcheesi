// Copyright Cats and Cubes. All Rights Reserved.

#include "UI/Widgets/CCSessionItemUI.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "OnlineSessionSettings.h"
#include "Blueprint/WidgetBlueprintLibrary.h"


FReply UCCSessionItemUI::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
    OnSessionItemPressedEvent.Broadcast(LocalSessionData);

    TArray<UUserWidget*> SessionWidgets;
    UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), SessionWidgets, SessionWidget, false);
    for (UUserWidget*  ItemWidget : SessionWidgets)
    {
        UCCSessionItemUI* MyNewWidget = Cast<UCCSessionItemUI>(ItemWidget);
        MyNewWidget->CleanSelection();
    }

    SelectingBorder->SetBrushColor(FLinearColor::Gray);
    return FReply::Handled();
}

void UCCSessionItemUI::SetSessionWidgetData(FString ServerName, int Players, int Ping)
{
    ServerNameText->SetText(FText::FromString(ServerName));
    PlayersText->SetText(FText::AsNumber(Players));
    PingText->SetText(FText::AsNumber(Ping));
}

void UCCSessionItemUI::SetSessionData(FOnlineSessionSearchResult SessionData)
{
    LocalSessionData = SessionData;
    SetSessionWidgetData(SessionData.GetSessionIdStr(), 1, SessionData.PingInMs);
    SelectingBorder->SetBrushColor(FLinearColor(0.5f, 0.5f, 0.5f, 0.2f));
}

void UCCSessionItemUI::CleanSelection()
{
    SelectingBorder->SetBrushColor(FLinearColor(0.5f, 0.5f, 0.5f, 0.2f));
}