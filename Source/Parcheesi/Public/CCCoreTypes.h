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
    bool TurnSatus = false;
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

USTRUCT(BlueprintType)
struct FAllPlayersData : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY()
    FText PlayerName;

    UPROPERTY()
    FName Tag;

    UPROPERTY()
    bool bIsReady;
};

USTRUCT(BlueprintType)
struct FPlayerInfo : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY()
    FName Tag;

    UPROPERTY()
    bool bIsReady;
};

USTRUCT(BlueprintType)
struct FGameSettings : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY()
    bool bMoveWithSix;

    UPROPERTY()
    int32 DicesToRool;
};

USTRUCT(BlueprintType)
struct FCameraMovemntData : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY()
    float FocalLength;
    
    UPROPERTY()
    FRotator SpringArmRotation;
    
    UPROPERTY()
    FRotator CameraRotation;
    
    UPROPERTY()
    FRotator ActorRotation;
    
    UPROPERTY()
    FVector ActorLocation;
};

USTRUCT(BlueprintType)
struct FLobbyPlayersData : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY()
    FText PlayerName;

    UPROPERTY()
    FUniqueNetIdRepl NetId;
};

UENUM(BlueprintType)
enum class ETurnColors : uint8
{
    Red,
    Green,
    Yellow,
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