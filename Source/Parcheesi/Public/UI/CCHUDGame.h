// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CCCoreTypes.h"
#include "CCHUDGame.generated.h"

class UCCGameLobbyUI;
class ACCPlayerPawnGame;
class UCCPlayersTurnContainer;
class UCCGameTurnButtons;
class UCCQuickMenuWidget;

UCLASS()
class PARCHEESI_API ACCHUDGame : public AHUD
{
    GENERATED_BODY()
protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> LobbyWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UCCPlayersTurnContainer> TurnInfoContainerWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UCCGameTurnButtons> GameTurnButtonsWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UCCQuickMenuWidget> QuickMenuWidgetClass;

    UFUNCTION()
    void StartGameFromLobby();

    UFUNCTION()
    void SelectColorInLobby(FName ColorTag);

    UFUNCTION()
    void EndPlayerTurn();

    UFUNCTION()
    void DebugEndPlayerTurn();

    UFUNCTION()
    void RollDice();

    UFUNCTION()
    void MovePawn();

    virtual void BeginPlay() override;

    void AddQuickMenuWidget();

public:
    void RemoveLobbyWidget();
    void UpdateTurnWidgets(const TArray<FPlayersTurnData>& PlayersTurnData);

    void ShowTurnButtons();
    void HideTurnButtons();

    void EnableEndTurnButton();

    void SwitchQuickMenuVisibility();
    void SwitchMovePawnButtonIsEnabled(bool State);
    void SetSeclectedDiceSideOnUI(int32 Side);


private:
    UPROPERTY()
    UCCGameLobbyUI* LobbyWidget = nullptr;

    UPROPERTY()
    UCCPlayersTurnContainer* TurnInfoContainerWidget = nullptr;

    UPROPERTY()
    UCCGameTurnButtons* GameTurnButtonsWidget = nullptr;

    UPROPERTY()
    UCCQuickMenuWidget* QuickMenuWidget = nullptr;

    UPROPERTY()
    ACCPlayerPawnGame* OwningPlayerPawn;
};
