// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CCCoreTypes.h"
#include "CCGameInstance.generated.h"

UCLASS()
class PARCHEESI_API UCCGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    void SetSessionName(FName Name) { SessionName = Name; }
    FName GetSessionName() { return SessionName; }

    void SetIsSinglePlayer(bool State) { bIsSinglePlayer = State; };
    bool GetIsSinglePlayer() { return bIsSinglePlayer; };

    void SetPlayerColor(ETurnColors Color) { PlayerColor = Color; };
    ETurnColors GetPlayerColor() { return PlayerColor; };

protected:
    UPROPERTY()
    FName SessionName;

    UPROPERTY()
    bool bIsSinglePlayer = false;

    UPROPERTY()
    ETurnColors PlayerColor = ETurnColors::None;
};
