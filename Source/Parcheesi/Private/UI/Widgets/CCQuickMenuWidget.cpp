// Copyright Cats and Cubes. All Rights Reserved.

#include "UI/Widgets/CCQuickMenuWidget.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Kismet/GameplayStatics.h"

void UCCQuickMenuWidget::NativeConstruct() {
    VB_Menu->SetVisibility(ESlateVisibility::Hidden);
    B_QuickMenu->OnClicked.AddDynamic(this, &UCCQuickMenuWidget::QuickMenuButtonClicked);
    B_Back->OnClicked.AddDynamic(this, &UCCQuickMenuWidget::BackButtonClicked);
    B_ExitToMenu->OnClicked.AddDynamic(this, &UCCQuickMenuWidget::ExitToMenuButtonClicked);
}

void UCCQuickMenuWidget::SwitchQuickMenuVisibility() {
    if (VB_Menu->GetVisibility() == ESlateVisibility::Hidden)
        VB_Menu->SetVisibility(ESlateVisibility::Visible);
    else
        VB_Menu->SetVisibility(ESlateVisibility::Hidden);
}

void UCCQuickMenuWidget::QuickMenuButtonClicked() {
    SwitchQuickMenuVisibility();
}

void UCCQuickMenuWidget::BackButtonClicked() {
    VB_Menu->SetVisibility(ESlateVisibility::Hidden);
}

void UCCQuickMenuWidget::ExitToMenuButtonClicked() {
    UGameplayStatics::OpenLevel(GetWorld(), FName("MenuMap"));

}
