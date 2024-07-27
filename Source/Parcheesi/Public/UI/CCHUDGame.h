// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CCHUDGame.generated.h"

class UCCGameLobbyUI;
class ACCGameModeBaseGame;

UCLASS()
class PARCHEESI_API ACCHUDGame : public AHUD
{
    GENERATED_BODY()
protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> LobbyWidgetClass;

    UPROPERTY()
    ACCGameModeBaseGame* ServerGameMode = nullptr;

    UFUNCTION()
    void StartGameFromLobby();

    virtual void BeginPlay() override;

public:
    UFUNCTION()
    void RemoveLobbyWidget();

private:
    UPROPERTY()
    UCCGameLobbyUI* LobbyWidget = nullptr;
};
