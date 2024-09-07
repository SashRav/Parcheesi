// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CCCoreTypes.generated.h"

class ACCPawn;

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


USTRUCT(BlueprintType)
struct FCellsData : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY()
    FVector CellPosition;

    UPROPERTY()
    ACCPawn* FirstPawnOnCell;

    UPROPERTY()
    ACCPawn* SecondPawnOnCell;
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