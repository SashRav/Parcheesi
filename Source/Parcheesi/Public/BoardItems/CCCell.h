// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CCCell.generated.h"

UStaticMeshComponent;

UCLASS()
class PARCHEESI_API ACCCell : public AActor
{
    GENERATED_BODY()

public:
    ACCCell();

    int32 GetCellIndex() { return CellIndex; }

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly)
    UStaticMeshComponent* CellMeshComponent;

    UPROPERTY(EditAnywhere)
    int32 CellIndex;
};
