// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "CCDice.generated.h"

class UBoxComponent;

UCLASS()
class PARCHEESI_API ACCDice : public AStaticMeshActor
{
    GENERATED_BODY()

public:
    int32 GetDiceSide();

    ACCDice();

protected:
    void GetHighestComponent();

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UBoxComponent* BoxSideFirst;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UBoxComponent* BoxSideSecond;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UBoxComponent* BoxSideThird;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UBoxComponent* BoxSideFourth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UBoxComponent* BoxSideFifth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UBoxComponent* BoxSideFSixth;
};
