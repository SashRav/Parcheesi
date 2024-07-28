// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CCControllerGame.generated.h"

class ACCPlayerStateGame;

UCLASS()
class PARCHEESI_API ACCControllerGame : public APlayerController
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;
};
