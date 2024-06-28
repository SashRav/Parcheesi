// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CCControllerLobby.generated.h"

/**
 *
 */
UCLASS()
class PARCHEESI_API ACCControllerLobby : public APlayerController
{
    GENERATED_BODY()
protected:

    virtual void BeginPlay() override;
};
