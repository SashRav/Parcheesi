// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CCCoreTypes.h"
#include "CCControllerGame.generated.h"

class ACCHUDGame;

UCLASS()
class PARCHEESI_API ACCControllerGame : public APlayerController
{
    GENERATED_BODY()

public:
    UFUNCTION(Client,Reliable)
    void Client_StartGameFromController();

    UFUNCTION(Client, Reliable)
    void Client_UpdateTurnWidgets(const TArray<FPlayersTurnData>& PlayersTurnData);

protected:
    UPROPERTY()
    ACCHUDGame* OwningHUD;

    virtual void BeginPlay() override;
};
