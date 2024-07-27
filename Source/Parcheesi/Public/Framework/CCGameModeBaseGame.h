// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CCGameModeBaseGame.generated.h"

UCLASS()
class PARCHEESI_API ACCGameModeBaseGame : public AGameModeBase
{
    GENERATED_BODY()
public:
    ACCGameModeBaseGame();

    void StartNewGame();

protected:
    virtual void BeginPlay() override;
};
