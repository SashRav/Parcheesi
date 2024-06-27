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
        SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UCCSearchGameUI::OnJoinSessionComplete);
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

void UCCSearchGameUI::JoinGameButtonClicked()
{
    if (SelectedSessionResult.IsValid())
    {
        StatusTextBox->SetText(FText::FromString("Connecting to the session"));
        SessionInterface->JoinSession(0, "", SelectedSessionResult);
    }
    else
        StatusTextBox->SetText(FText::FromString("No Session Selected"));
}

void UCCSearchGameUI::BackToMenuButtonlicked()
{
    FOnlineSessionSearchResult NewSelectedSessionResult;
    SelectedSessionResult = NewSelectedSessionResult;
    MainMenuHUD->ShowMainMenuWidget();
}

void UCCSearchGameUI::AddServerLine(FOnlineSessionSearchResult Result)
{
    SessionWidget = CreateWidget<UCCSessionItemUI>(GetWorld(), SessionResultWidgetClass);
    SessionWidget->SetSessionData(Result);
    SessionWidget->OnSessionItemPressedEvent.AddUObject(this, &UCCSearchGameUI::SetSelectedSession);

    ServerScrollBox->AddChild(SessionWidget);
}

void UCCSearchGameUI::OnFindSessionsComplete(bool Succeeded)
{
    if (Succeeded)
    {
        TArray<FOnlineSessionSearchResult> LocalSearchResults = SessionSearch->SearchResults;

        if (LocalSearchResults.Num() != 0)
        {
            StatusTextBox->SetText(FText::FromString("Sessions available:"));

            for (FOnlineSessionSearchResult Result : LocalSearchResults)
            {
                AddServerLine(Result);
            }
        }
        else
        {
            StatusTextBox->SetText(FText::FromString("No Servers Available"));
        }
    }
}

void UCCSearchGameUI::SetSelectedSession(FOnlineSessionSearchResult Result)
{
    SelectedSessionResult = Result;
}

void UCCSearchGameUI::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PlayerController)
    {
        FString JoinAddress;
        SessionInterface->GetResolvedConnectString(SessionName, JoinAddress);
        if (JoinAddress != "")
            PlayerController->ClientTravel(JoinAddress, ETravelType::TRAVEL_Absolute);
    }
}
