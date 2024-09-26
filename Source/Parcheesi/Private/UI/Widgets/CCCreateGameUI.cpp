// Copyright Cats and Cubes. All Rights Reserved.

#include "UI/Widgets/CCCreateGameUI.h"
#include "UI/Widgets/CCMenuButtonUI.h"
#include "UI/CCHUDMainMenu.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Components/EditableTextBox.h"
#include "Components/CheckBox.h"
#include "Framework/CCGameInstance.h"

void UCCCreateGameUI::NativeConstruct()
{
    const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PlayerController)
        MainMenuHUD = Cast<ACCHUDMainMenu>(PlayerController->GetHUD());

    B_CreateSession->OnButtonPressedEvent.AddDynamic(this, &UCCCreateGameUI::CreateSessionButtonClicked);
    B_BackToMenu->OnButtonPressedEvent.AddDynamic(this, &UCCCreateGameUI::BackToMenuButtonlicked);
    ET_MaxPlayers->OnTextChanged.AddDynamic(this, &UCCCreateGameUI::MaxPlayerNuberChanged);

    B_ApplyName->SetIsEnabled(false);
    ET_PlayerName->SetIsEnabled(false);
    ET_MaxPlayers->SetText(FText::FromString("4"));
    ET_ServerName->SetText(FText::FromString("MySession"));
    C_UseLan->SetIsChecked(true);

     SessionInterface = Online::GetSessionInterface(GetWorld());
    if (SessionInterface.IsValid())
        SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UCCCreateGameUI::OnCreateSessionComplete);
}

void UCCCreateGameUI::CreateSessionButtonClicked()
{
    CreateSession();
}

void UCCCreateGameUI::BackToMenuButtonlicked()
{
    MainMenuHUD->ShowMainMenuWidget();
}

void UCCCreateGameUI::CreateSession()
{
    int32 NumPublicConnections = FCString::Atoi(*ET_MaxPlayers->GetText().ToString());
    bool IsLANMatch = C_UseLan->IsChecked();
    FString SessoinNameString = ET_ServerName->GetText().ToString();
    FName SessionName = FName(*SessoinNameString);

    FOnlineSessionSettings SessionSettings;
    SessionSettings.bAllowJoinInProgress = true;
    SessionSettings.bShouldAdvertise = true;
    SessionSettings.bUsesPresence = true;
    SessionSettings.NumPublicConnections = NumPublicConnections;
    SessionSettings.bIsLANMatch = IsLANMatch;
    SessionSettings.Set(FName("SESSION_NAME_KEY"), SessoinNameString, EOnlineDataAdvertisementType::ViaOnlineService);

    if (SessionInterface.IsValid())
        SessionInterface->CreateSession(0, SessionName, SessionSettings);

    if (UCCGameInstance* GameInstance = Cast<UCCGameInstance>(GetGameInstance()))
        GameInstance->SetSessionName(SessionName);

}

void UCCCreateGameUI::OnCreateSessionComplete(FName ServerName, bool Succeeded)
{
    if (Succeeded)
        GetWorld()->ServerTravel("/Game/_Main/Maps/GameMap?listen");
}

void UCCCreateGameUI::MaxPlayerNuberChanged(const FText& Text)
{
    FString EnteredText = Text.ToString();
    int32 EnteredValue = FCString::Atoi(*EnteredText);
    EnteredValue = FMath::Clamp(EnteredValue, 2, 8); // Set max value of players between 2 to 8
    FString CorrectedText = FString::FromInt(EnteredValue);
    ET_MaxPlayers->SetText(FText::FromString(CorrectedText));
}
