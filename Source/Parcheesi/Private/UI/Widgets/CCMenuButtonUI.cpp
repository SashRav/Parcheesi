// Copyright Cats and Cubes. All Rights Reserved.

#include "UI/Widgets/CCMenuButtonUI.h"
#include "Components/Button.h"

void UCCMenuButtonUI::NativeConstruct()
{
    Button->OnClicked.AddDynamic(this, &UCCMenuButtonUI::ButtonClicked);
}

void UCCMenuButtonUI::ButtonClicked()
{
    OnButtonPressedEvent.Broadcast();
}



