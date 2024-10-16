// Copyright Cats and Cubes. All Rights Reserved.

#include "UI/Widgets/CCMenuSingleplayerUI.h"
#include "UI/Widgets/CCMenuButtonUI.h"
#include "UI/CCHUDMainMenu.h"
#include "Kismet/GameplayStatics.h"

void UCCMenuSingleplayerUI::NativeConstruct()
{
    const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PlayerController)
        MainMenuHUD = Cast<ACCHUDMainMenu>(PlayerController->GetHUD());

    B_StartGame->OnButtonPressedEvent.AddDynamic(this, &UCCMenuSingleplayerUI::StartGameButtonClicked);
    B_BackToMenu->OnButtonPressedEvent.AddDynamic(this, &UCCMenuSingleplayerUI::BackToMenuButtonlicked);
}

void UCCMenuSingleplayerUI::StartGameButtonClicked()
{
    StartGame();
}

void UCCMenuSingleplayerUI::BackToMenuButtonlicked()
{
    MainMenuHUD->ShowMainMenuWidget();
}

void UCCMenuSingleplayerUI::StartGame() 
{
    UGameplayStatics::OpenLevel(this, "/Game/_Main/Maps/GameMap");
}
