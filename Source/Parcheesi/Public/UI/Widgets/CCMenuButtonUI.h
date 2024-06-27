// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CCMenuButtonUI.generated.h"

class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnButtonPressed);

UCLASS()
class PARCHEESI_API UCCMenuButtonUI : public UUserWidget
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable)
    FOnButtonPressed OnButtonPressedEvent;

protected:
    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    class UButton* Button;

    UFUNCTION()
    void ButtonClicked();

};
