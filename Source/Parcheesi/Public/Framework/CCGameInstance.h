// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CCGameInstance.generated.h"

UCLASS()
class PARCHEESI_API UCCGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    void SetSessionName(FName Name) { SessionName = Name; }
    FName GetSessionName() { return SessionName; }

protected:
    UPROPERTY()
    FName SessionName;
};
