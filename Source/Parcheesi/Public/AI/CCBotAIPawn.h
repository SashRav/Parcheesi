// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CCBotAIPawn.generated.h"

UCLASS()
class PARCHEESI_API ACCBotAIPawn : public APawn
{
    GENERATED_BODY()

public:
    ACCBotAIPawn();

    void SetBotTagName(FName TagName) { BotTagName = TagName; };
    FName GetBotTagName() { return BotTagName; }

protected:
    virtual void BeginPlay() override;

    UPROPERTY()
    FName BotTagName;
};
