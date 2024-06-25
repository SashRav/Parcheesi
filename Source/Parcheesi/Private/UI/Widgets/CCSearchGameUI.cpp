// Copyright Cats and Cubes. All Rights Reserved.

#include "UI/Widgets/CCSearchGameUI.h"
#include "UI/Widgets/CCMenuButtonUI.h"
#include "UI/Widgets/CCSessionItemUI.h"
#include "UI/CCHUDMainMenu.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Online/OnlineSessionNames.h"

void UCCSearchGameUI::NativeConstruct()
{
    check(SessionResultWidgetClass)

        const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PlayerController)
        MainMenuHUD = Cast<ACCHUDMainMenu>(PlayerController->GetHUD());

    StatusTextBox->SetText(FText::FromString("Games to Join"));
    RefreshButton->OnButtonPressedEvent.AddDynamic(this, &UCCSearchGameUI::RefreshButtonClicked);
    JoinGameButton->OnButtonPressedEvent.AddDynamic(this, &UCCSearchGameUI::JoinGameButtonClicked);
    BackToMenuButton->OnButtonPressedEvent.AddDynamic(this, &UCCSearchGameUI::BackToMenuButtonlicked);

    SessionInterface = Online::GetSessionInterface(GetWorld());
    if (SessionInterface.IsValid())
    {
        SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UCCSearchGameUI::OnFindSessionsComplete);
    }
}

void UCCSearchGameUI::RefreshButtonClicked()
{
    ServerScrollBox->ClearChildren();
    StatusTextBox->SetText(FText::FromString("Searching..."));

    SessionSearch.Reset();
    SessionSearch = MakeShareable(new FOnlineSessionSearch());
    SessionSearch->MaxSearchResults = 100; // For now 100 is enough for debug
    SessionSearch->bIsLanQuery = false;
    SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

    if (SessionInterface.IsValid())
    {
        SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
    }
}

void UCCSearchGameUI::JoinGameButtonClicked() {}

void UCCSearchGameUI::BackToMenuButtonlicked()
{
    MainMenuHUD->ShowMainMenuWidget();
}

void UCCSearchGameUI::AddServerLine(FString ServerName, int Players, int Ping)
{
    SessionWidget = CreateWidget<UCCSessionItemUI>(GetWorld(), SessionResultWidgetClass);
    SessionWidget->SetSessionWidgetData(FText::FromString(ServerName), Players, Ping);
    ServerScrollBox->AddChild(SessionWidget);
}

void UCCSearchGameUI::OnFindSessionsComplete(bool Succeeded)
{
    if (Succeeded)
    {
        TArray<FOnlineSessionSearchResult> SearchResults = SessionSearch->SearchResults;

        if (SearchResults.Num() != 0)
        {
            StatusTextBox->SetText(FText::FromString("Sessions available:"));
            for (FOnlineSessionSearchResult Result : SearchResults)
            {
                AddServerLine("Test Server", 1, Result.PingInMs);
            }
        }
        else
        {
            StatusTextBox->SetText(FText::FromString("No Servers Available"));
        }
    }
}