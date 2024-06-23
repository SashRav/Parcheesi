// Copyright Cats and Cubes. All Rights Reserved.

#include "UI/Widgets/CCSearchGameUI.h"
#include "UI/Widgets/CCMenuButtonUI.h"
#include "UI/Widgets/CCSessionItemUI.h"
#include "UI/CCHUDMainMenu.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ScrollBox.h"

void UCCSearchGameUI::NativeConstruct()
{
    check(SessionResultWidgetClass)

        const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PlayerController)
        MainMenuHUD = Cast<ACCHUDMainMenu>(PlayerController->GetHUD());

    RefreshButton->OnButtonPressedEvent.AddDynamic(this, &UCCSearchGameUI::RefreshButtonClicked);
    JoinGameButton->OnButtonPressedEvent.AddDynamic(this, &UCCSearchGameUI::JoinGameButtonClicked);

    BackToMenuButton->OnButtonPressedEvent.AddDynamic(this, &UCCSearchGameUI::BackToMenuButtonlicked);
}

void UCCSearchGameUI::RefreshButtonClicked()
{
    SessionWidget = CreateWidget<UCCSessionItemUI>(GetWorld(), SessionResultWidgetClass);
    SessionWidget->SetSessionWidgetData(FText::FromString("Test"), 1, 1);
    ServerScrollBox->AddChild(SessionWidget);
}

void UCCSearchGameUI::JoinGameButtonClicked() {}

void UCCSearchGameUI::BackToMenuButtonlicked()
{
    MainMenuHUD->ShowMainMenuWidget();
}
