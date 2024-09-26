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
    for (UUserWidget* ItemWidget : SessionWidgets)
    {
        UCCSessionItemUI* MyNewWidget = Cast<UCCSessionItemUI>(ItemWidget);
        MyNewWidget->CleanSelection();
    }

    SelectingBorder->SetBrushColor(FLinearColor::Gray);
    return FReply::Handled();
}

void UCCSessionItemUI::SetSessionWidgetData(FName ServerName, int Players, int MaxPlayers, int Ping)
{
    ServerNameText->SetText(FText::FromName(ServerName));
    
    FString PlayersData = FString::FromInt(Players) + " / " + FString::FromInt(MaxPlayers);
    PlayersText->SetText(FText::FromString(*PlayersData));
    PingText->SetText(FText::AsNumber(Ping));
}

void UCCSessionItemUI::SetSessionData(FOnlineSessionSearchResult SessionData)
{
    LocalSessionData = SessionData;
    int32 MaxPlayers = LocalSessionData.Session.SessionSettings.NumPublicConnections;
    int32 CurrentPlayers = MaxPlayers - LocalSessionData.Session.NumOpenPublicConnections;

    FString SessionNameString;

    if (!LocalSessionData.Session.SessionSettings.Get(FName("SESSION_NAME_KEY"), SessionNameString))
        SessionNameString = "Underfind Session";

    FName SessionName = FName(*SessionNameString);

    SetSessionWidgetData(SessionName, CurrentPlayers, MaxPlayers, SessionData.PingInMs);
    SelectingBorder->SetBrushColor(FLinearColor(0.5f, 0.5f, 0.5f, 0.2f));
}

void UCCSessionItemUI::CleanSelection()
{
    SelectingBorder->SetBrushColor(FLinearColor(0.5f, 0.5f, 0.5f, 0.2f));
}