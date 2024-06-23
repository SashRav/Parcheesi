// Copyright Cats and Cubes. All Rights Reserved.

#include "UI/Widgets/CCMultiplayerUI.h"
#include "UI/Widgets/CCMenuButtonUI.h"
#include "UI/CCHUDMainMenu.h"
#include "Kismet/GameplayStatics.h"


void UCCMultiplayerUI::NativeConstruct() {
    const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PlayerController)
        MainMenuHUD = Cast<ACCHUDMainMenu>(PlayerController->GetHUD());

    // Binding evenents for buttons click
    CreateGameButton->OnButtonPressedEvent.AddDynamic(this, &UCCMultiplayerUI::CreateGameButtonClicked);
    JoinGameButton->OnButtonPressedEvent.AddDynamic(this, &UCCMultiplayerUI::JoinGameButtonClicked);
    BackToMenuButton->OnButtonPressedEvent.AddDynamic(this, &UCCMultiplayerUI::BackToMenuButtonlicked);
}

void UCCMultiplayerUI::CreateGameButtonClicked()
{
    MainMenuHUD->ShowCreateGameWidget();
}

void UCCMultiplayerUI::JoinGameButtonClicked()
{
    MainMenuHUD->ShowJoinGameWidget();
}

void UCCMultiplayerUI::BackToMenuButtonlicked()
{
    MainMenuHUD->ShowMainMenuWidget();
}
