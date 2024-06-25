// Copyright Cats and Cubes. All Rights Reserved.

#include "UI/Widgets/CCCreateGameUI.h"
#include "UI/Widgets/CCMenuButtonUI.h"
#include "UI/CCHUDMainMenu.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"

void UCCCreateGameUI::NativeConstruct()
{
    const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PlayerController)
        MainMenuHUD = Cast<ACCHUDMainMenu>(PlayerController->GetHUD());

    CreateSessionButton->OnButtonPressedEvent.AddDynamic(this, &UCCCreateGameUI::CreateSessionButtonClicked);
    BackToMenuButton->OnButtonPressedEvent.AddDynamic(this, &UCCCreateGameUI::BackToMenuButtonlicked);

    SessionInterface = Online::GetSessionInterface(GetWorld());
    if (SessionInterface.IsValid())
    {
        SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UCCCreateGameUI::OnCreateSessionComplete);
    }
}

void UCCCreateGameUI::CreateSessionButtonClicked()
{
    CreateSession(5, true);
}

void UCCCreateGameUI::BackToMenuButtonlicked()
{
    MainMenuHUD->ShowMainMenuWidget();
}

void UCCCreateGameUI::CreateSession(int32 NumPublicConnections, bool IsLANMatch)
{
    FOnlineSessionSettings SessionSettings;
    SessionSettings.bAllowJoinInProgress = true;
    SessionSettings.bShouldAdvertise = true;
    SessionSettings.bIsDedicated = false;
    SessionSettings.bUsesPresence = false;
    SessionSettings.NumPublicConnections = NumPublicConnections;
    SessionSettings.bIsLANMatch = IsLANMatch;

    if (SessionInterface.IsValid())
    {
        SessionInterface->CreateSession(0, FName("My session"), SessionSettings);
    }
}

void UCCCreateGameUI::OnCreateSessionComplete(FName ServerName, bool Succeeded)
{
    if (Succeeded)
        GetWorld()->ServerTravel("/Game/_Main/Maps/GameMap?listen"); // UGameplayStatics::OpenLevel(GetWorld(), FName("GameMap?listen"));
}
