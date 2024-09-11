// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CCCoreTypes.h"
#include "CCControllerGame.generated.h"

class ACCHUDGame;
class UInputMappingContext;
class UInputAction;

UCLASS()
class PARCHEESI_API ACCControllerGame : public APlayerController
{
    GENERATED_BODY()

public:
    UFUNCTION(Client, Reliable)
    void Client_StartGameFromController();

    UFUNCTION(Client, Reliable)
    void Client_UpdateTurnWidgets(const TArray<FPlayersTurnData>& PlayersTurnData);

    UFUNCTION(Client, Reliable)
    void Client_ShowTurnButtonsWidget();

    UFUNCTION(Client, Reliable)
    void Client_HideTurnButtonsWidget();

    UFUNCTION(Client, Reliable)
    void Client_SwitchQuickMenuVisibility();

    UFUNCTION(Client, Reliable)
    void Client_EnableEndTurnButton();

    UFUNCTION(Client, Reliable)
    void Client_SwitchMovePawnButtonIsEnabled(bool State);

    UFUNCTION(Client, Reliable)
    void Client_SetDiceSideOnUI(int32 Side);

    UFUNCTION(Client, Reliable)
    void Client_ShowWinWidget(const FText& WinnerName);

protected:
    UPROPERTY()
    ACCHUDGame* OwningHUD;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputMappingContext* GameMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputMappingContext* LevelInputContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* QuickMenuAction;

    virtual void BeginPlay() override;

    virtual void SetupInputComponent() override;
};
