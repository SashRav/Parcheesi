// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CCGameLobbyUI.generated.h"

class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartGameButtonPressed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnColorButtonPressed, FName, ColorTag);

    UCLASS() class PARCHEESI_API UCCGameLobbyUI : public UUserWidget
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable)
    FOnStartGameButtonPressed OnStartGameButtonPressedEvent;

    UPROPERTY(BlueprintAssignable)
    FOnColorButtonPressed OnColorButtonPressed;

protected:
    UPROPERTY(meta = (BindWidget))
    class UButton* StartGameButton;

    UPROPERTY(meta = (BindWidget))
    class UButton* SelectRedButton;

    UPROPERTY(meta = (BindWidget))
    class UButton* SelectYellowButton;

    UPROPERTY(meta = (BindWidget))
    class UButton* SelectGreenButton;

    UPROPERTY(meta = (BindWidget))
    class UButton* SelectBlueButton;

    UFUNCTION()
    void StartGameButtonClicked();

    UFUNCTION()
    void SelectRedButtonClicked();

    UFUNCTION()
    void SelectYellowButtonClicked();

    UFUNCTION()
    void SelectGreenButtonClicked();

    UFUNCTION()
    void SelectBlueButtonClicked();

    virtual void NativeConstruct() override;
};
