// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CCHUDLobby.generated.h"

class UCCGameLobbyUI;

UCLASS()
class PARCHEESI_API ACCHUDLobby : public AHUD
{
    GENERATED_BODY()

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> LobbyWidgetClass;

    UFUNCTION()
    void StartGameFromLobby();

    virtual void BeginPlay() override;

private:
    UPROPERTY()
    UCCGameLobbyUI* LobbyWidget = nullptr;
};
