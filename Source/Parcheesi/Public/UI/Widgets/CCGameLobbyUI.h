// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CCGameLobbyUI.generated.h"

class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartGameButtonPressed);

UCLASS()
class PARCHEESI_API UCCGameLobbyUI : public UUserWidget
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable)
    FOnStartGameButtonPressed OnStartGameButtonPressedEvent;

protected:
    UPROPERTY(meta = (BindWidget))
    class UButton* StartGameButton;

    UFUNCTION()
    void StartGameButtonClicked();

    virtual void NativeConstruct() override;
};
