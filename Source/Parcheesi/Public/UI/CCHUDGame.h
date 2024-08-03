// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CCCoreTypes.h"
#include "CCHUDGame.generated.h"

class UCCGameLobbyUI;
class ACCPlayerPawnGame;
class UCCPlayersTurnContainer;

UCLASS()
class PARCHEESI_API ACCHUDGame : public AHUD
{
    GENERATED_BODY()
protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> LobbyWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UCCPlayersTurnContainer> TurnInfoContainerWidgetClass;

    UFUNCTION()
    void StartGameFromLobby();

    UFUNCTION()
    void SelectColorInLobby(FName ColorTag);

    virtual void BeginPlay() override;

public:
    UFUNCTION()
    void RemoveLobbyWidget();

    UFUNCTION()
    void UpdateTurnWidgets(const TArray<FPlayersTurnData>& PlayersTurnData);

private:
    UPROPERTY()
    UCCGameLobbyUI* LobbyWidget = nullptr;

    UPROPERTY()
    UCCPlayersTurnContainer* TurnInfoContainerWidget = nullptr;

    UPROPERTY()
    ACCPlayerPawnGame* OwningPlayerPawn;
};
