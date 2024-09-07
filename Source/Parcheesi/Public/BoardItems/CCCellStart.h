// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BoardItems/CCCell.h"
#include "CCCellStart.generated.h"

UCLASS()
class PARCHEESI_API ACCCellStart : public ACCCell
{
    GENERATED_BODY()

public:
    int32 GetClosestBoardCellIndex() { return ClosestBoardCellIndex; }
    int32 GetClosestFinishCellIndex() { return ClosestFinishCellIndex; }

protected:
    UPROPERTY(EditAnywhere)
    int32 ClosestBoardCellIndex = -1;

    UPROPERTY(EditAnywhere)
    int32 ClosestFinishCellIndex = -1;
};
