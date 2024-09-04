// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CCCoreTypes.generated.h"

USTRUCT(BlueprintType)
struct FPlayersTurnData : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY()
    FString PlayerName;

    UPROPERTY()
    FLinearColor PlayerColor;

    UPROPERTY()
    bool TurnSatus;
};

UENUM(BlueprintType)
enum class ETurnColors : uint8
{
    Red,
    Yellow,
    Green,
    Blue,
    None
};

UENUM(BlueprintType)
enum class EPawnPosition : uint8
{
    OnStart,
    OnBoard,
    OnFinish
};