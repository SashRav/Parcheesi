// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CCControllerMainMenu.generated.h"

/**
 *
 */
UCLASS()
class PARCHEESI_API ACCControllerMainMenu : public APlayerController
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;
};
