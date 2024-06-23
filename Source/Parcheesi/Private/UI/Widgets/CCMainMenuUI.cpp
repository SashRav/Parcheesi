// Copyright Cats and Cubes. All Rights Reserved.

#include "UI/Widgets/CCMainMenuUI.h"
#include "UI/Widgets/CCMenuButtonUI.h"
#include "UI/CCHUDMainMenu.h"
#include "Kismet/GameplayStatics.h"

void UCCMainMenuUI::NativeConstruct()
{
    const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PlayerController)
        MainMenuHUD = Cast<ACCHUDMainMenu>(PlayerController->GetHUD());

    // Binding evenents for buttons click
    SingleplayerButton->OnButtonPressedEvent.AddDynamic(this, &UCCMainMenuUI::SingleplayerButtonClicked);
    MultiplayerButton->OnButtonPressedEvent.AddDynamic(this, &UCCMainMenuUI::MultiplayerButtonClicked);
    SettingsButton->OnButtonPressedEvent.AddDynamic(this, &UCCMainMenuUI::SettingsButtonClicked);
    ExitButton->OnButtonPressedEvent.AddDynamic(this, &UCCMainMenuUI::ExitButtonClicked);
}

void UCCMainMenuUI::SingleplayerButtonClicked()
{
    // MainMenuHUD->ShowSingleplayerWidget(); // Will be enabled when single player widget will be ready
}

void UCCMainMenuUI::MultiplayerButtonClicked()
{
    MainMenuHUD->ShowMultiplayerWidget();
}

void UCCMainMenuUI::SettingsButtonClicked()
{
    // MainMenuHUD->ShowSettingsWidget(); // Will be enabled when single player widget will be ready
}

void UCCMainMenuUI::ExitButtonClicked()
{
    UGameplayStatics::GetPlayerController(GetWorld(), 0)->ConsoleCommand("Quit");
}
