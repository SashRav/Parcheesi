// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CCCoreTypes.generated.h"

USTRUCT(BlueprintType)
struct FPlayersData : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Players")
    FUniqueNetIdRepl PlayerNetId;

    // Time between enemy attacks
    UPROPERTY(EditDefaultsOnly, Category = "Players")
    FName PlayerTag;
};