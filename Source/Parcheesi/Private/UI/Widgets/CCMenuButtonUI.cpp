// Copyright Cats and Cubes. All Rights Reserved.


#include "UI/Widgets/CCMenuButtonUI.h"
#include "Components/Button.h"

bool UCCMenuButtonUI::Initialize()
{
    Super::Initialize();
    Button->OnClicked.AddDynamic(this, &UCCMenuButtonUI::ButtonClicked);
    return true;
}

void UCCMenuButtonUI::ButtonClicked() {
    OnButtonPressedEvent.Broadcast();
}
