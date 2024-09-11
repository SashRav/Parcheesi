// Copyright Cats and Cubes. All Rights Reserved.


#include "UI/Widgets/CCWinWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UCCWinWidget::NativeConstruct()
{
    B_BackMenu->OnClicked.AddDynamic(this, &UCCWinWidget::BackToMenuButtonClicked);
}

void UCCWinWidget::SetWinner(FText Name)
{
    T_WinPlayer->SetText(Name);
}

void UCCWinWidget::BackToMenuButtonClicked() {
    UGameplayStatics::OpenLevel(GetWorld(), FName("MenuMap"));
}


