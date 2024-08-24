// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CCGameTurnButtons.generated.h"

class UButton;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRollDicePressed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMovePawnPressed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndTurnPressed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDebugEndTurnPressed);

UCLASS()
class PARCHEESI_API UCCGameTurnButtons : public UUserWidget
{
    GENERATED_BODY()

public:
    void EnableRollButton();

    UPROPERTY(BlueprintAssignable)
    FOnMovePawnPressed OnMovePawnPressedEvent;

    UPROPERTY(BlueprintAssignable)
    FOnRollDicePressed OnRollDicePressedEvent;

    UPROPERTY(BlueprintAssignable)
    FOnEndTurnPressed OnEndTurnPressedEvent;

    UPROPERTY(BlueprintAssignable)
    FOnDebugEndTurnPressed OnDebugEndTurnPressedEvent;

protected:
    UPROPERTY(meta = (BindWidget))
    class UButton* B_RollDice;

    UPROPERTY(meta = (BindWidget))
    class UButton* B_MovePawn;

    UPROPERTY(meta = (BindWidget))
    class UButton* B_EndTurn;

    UPROPERTY(meta = (BindWidget))
    class UButton* B_DebugEndTurn;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* T_DiceSide;

    UFUNCTION()
    void RollDiceButtonClicked();

    UFUNCTION()
    void MovePawnButtonClicked();

    UFUNCTION()
    void EndTurnClicked();

    UFUNCTION()
    void DebugEndTurnClicked();

    virtual void NativeConstruct() override;
};
