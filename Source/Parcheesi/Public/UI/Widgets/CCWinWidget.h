// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CCWinWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMenuFromWinButtonPressed);

class UButton;
class UTextBlock;

UCLASS()
class PARCHEESI_API UCCWinWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void SetWinner(FText Name);

    UPROPERTY(BlueprintAssignable)
    FOnMenuFromWinButtonPressed OnMenuFromWinButtonPressed;

protected:
    UPROPERTY(meta = (BindWidget))
    class UButton* B_BackMenu;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* T_WinPlayer;

    UFUNCTION()
    void BackToMenuButtonClicked();

     virtual void NativeConstruct() override;
};
