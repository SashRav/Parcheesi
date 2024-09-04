// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CCDiceComponent.generated.h"

class ACCDice;
class ACCGameStateGame;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDiceRollingEnd);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PARCHEESI_API UCCDiceComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    void SpawnDice(FVector SpawnLocation, FRotator Rotation, bool UseVelocity, bool SimulatePhysics);
    void RollDices();

    UPROPERTY(BlueprintAssignable)
    FOnDiceRollingEnd OnDiceRollingEnd;

protected:
    virtual void BeginPlay() override;

    void SetDiceVelocity(ACCDice* Dice);
    void TryDoubleDices();

    UFUNCTION()
    void MoveDicesToBoard(TArray<FVector> TargetLocations);

    UPROPERTY()
    ACCGameStateGame* ServerGameState;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<ACCDice> DiceClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    TMap<int32, FRotator> DiceSidesRotation;

    UPROPERTY()
    TArray<FVector> DicePlacesLocation;

    UPROPERTY()
    TArray<FVector> DoubledDicePlacesLocation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName DicePlaceTag = "DicePlace";

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName DoubledDicePlaceTag = "DicePlaceDoubled";

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DiceMoveDelay = 5.0f;
};
