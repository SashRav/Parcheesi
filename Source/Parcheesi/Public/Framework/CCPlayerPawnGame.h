// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CCPlayerPawnGame.generated.h"

class ACCGameModeBaseGame;

UCLASS()
class PARCHEESI_API ACCPlayerPawnGame : public APawn
{
    GENERATED_BODY()

public:
    ACCPlayerPawnGame();

    UFUNCTION(Server, Reliable)
    void Server_UpdateSelectedColor(const FName& ColorTag);

protected:
    virtual void BeginPlay() override;

    UPROPERTY()
    ACCGameModeBaseGame* ServerGameMode;
};
