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

protected:
    UPROPERTY(EditAnywhere)
    int32 ClosestBoardCellIndex = -1;
};
