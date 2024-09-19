// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CCQuickMenuWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMenuButtonPressed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBackButtonPressed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExitToMenuButtonPressed);

class UButton;
class UVerticalBox;

UCLASS()
class PARCHEESI_API UCCQuickMenuWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable)
    FOnMenuButtonPressed FOnMenuButtonPressedEvent;

    UPROPERTY(BlueprintAssignable)
    FOnBackButtonPressed FOnBackButtonPressedEvent;

    UPROPERTY(BlueprintAssignable)
    FOnExitToMenuButtonPressed OnExitToMenuButtonPressedEvent;

    void SwitchQuickMenuVisibility();

protected:
    UPROPERTY(meta = (BindWidget))
    class UButton* B_QuickMenu;

    UPROPERTY(meta = (BindWidget))
    class UButton* B_Back;

    UPROPERTY(meta = (BindWidget))
    class UButton* B_ExitToMenu;

    UPROPERTY(meta = (BindWidget))
    class UVerticalBox* VB_Menu;

    UFUNCTION()
    void QuickMenuButtonClicked();

    UFUNCTION()
    void BackButtonClicked();

    UFUNCTION()
    void ExitToMenuButtonClicked();

    virtual void NativeConstruct() override;
};
