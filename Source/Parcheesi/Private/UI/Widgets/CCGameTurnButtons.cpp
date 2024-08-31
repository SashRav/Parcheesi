// Copyright Cats and Cubes. All Rights Reserved.


#include "UI/Widgets/CCGameTurnButtons.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"


void UCCGameTurnButtons::NativeConstruct() {
    B_RollDice->OnClicked.AddDynamic(this, &UCCGameTurnButtons::RollDiceButtonClicked);
    B_MovePawn->OnClicked.AddDynamic(this, &UCCGameTurnButtons::MovePawnButtonClicked);
    B_EndTurn->OnClicked.AddDynamic(this, &UCCGameTurnButtons::EndTurnClicked);
    B_DebugEndTurn->OnClicked.AddDynamic(this, &UCCGameTurnButtons::DebugEndTurnClicked);
}

void UCCGameTurnButtons::RollDiceButtonClicked()
{
    OnRollDicePressedEvent.Broadcast();
    B_RollDice->SetIsEnabled(false);
}

void UCCGameTurnButtons::MovePawnButtonClicked() {
    OnMovePawnPressedEvent.Broadcast();
}

void UCCGameTurnButtons::EndTurnClicked() {
    OnEndTurnPressedEvent.Broadcast();
    this->SetVisibility(ESlateVisibility::Hidden);
}

void UCCGameTurnButtons::DebugEndTurnClicked() {
    OnDebugEndTurnPressedEvent.Broadcast();
    this->SetVisibility(ESlateVisibility::Hidden);
}

void UCCGameTurnButtons::EnableRollButton() {
    B_RollDice->SetIsEnabled(true);
}

void UCCGameTurnButtons::SetIsEnabledEndTurnButton(bool State)
{
    B_EndTurn->SetIsEnabled(State);
}

void UCCGameTurnButtons::SetDiceSideOnUI(int32 Side) {
    T_DiceSide->SetText(FText::FromString(FString::FromInt(Side)));
}
