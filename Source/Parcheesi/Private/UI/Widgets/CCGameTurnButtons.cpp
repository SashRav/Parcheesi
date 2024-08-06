// Copyright Cats and Cubes. All Rights Reserved.


#include "UI/Widgets/CCGameTurnButtons.h"
#include "Components/Button.h"


void UCCGameTurnButtons::NativeConstruct() {
    B_RollDice->OnClicked.AddDynamic(this, &UCCGameTurnButtons::RollDiceButtonClicked);
    B_MovePawn->OnClicked.AddDynamic(this, &UCCGameTurnButtons::MovePawnButtonClicked);
    B_EndTurn->OnClicked.AddDynamic(this, &UCCGameTurnButtons::EndTurnClicked);
    B_DebugEndTurn->OnClicked.AddDynamic(this, &UCCGameTurnButtons::DebugEndTurnClicked);
}

void UCCGameTurnButtons::RollDiceButtonClicked() {
    OnRollDicePressedEvent.Broadcast();
}

void UCCGameTurnButtons::MovePawnButtonClicked() {
    FOnMovePawnPressedEvent.Broadcast();
}

void UCCGameTurnButtons::EndTurnClicked() {
    OnEndTurnPressedEvent.Broadcast();
    this->SetVisibility(ESlateVisibility::Hidden);
}

void UCCGameTurnButtons::DebugEndTurnClicked() {
    OnDebugEndTurnPressedEvent.Broadcast();
    this->SetVisibility(ESlateVisibility::Hidden);
}

