// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CCCoreTypes.h"
#include "CCPawn.generated.h"

class UStaticMeshComponent;
class USplineComponent;

UCLASS()
class PARCHEESI_API ACCPawn : public AActor
{
    GENERATED_BODY()

public:
    ACCPawn();

    void SetCurrentPawnPosition(EPawnPosition Position) { CurrentPawnPosition = Position; }
    EPawnPosition GetCurrentPawnPosition() { return CurrentPawnPosition; }

    int32 GetStartCellIndex() { return StartCellIndex; }
    int32 GetCurrentCellIndex() { return CurrentCellIndex; }
    int32 GetFirstBoardCellIndex() { return FirstBoardCellIndex; }

    void SetCurrentCellIndex(int32 Index) { CurrentCellIndex = Index; }


    UFUNCTION(NetMulticast, Reliable)
    void Multicast_SetupPawnData(ETurnColors PawnColor, int32 StartCell, int32 FirstBoardCell);

    UPROPERTY(EditDefaultsOnly)
    UStaticMeshComponent* PawnMeshComponent;

    UPROPERTY(EditAnywhere)
    USplineComponent* SplienComponent;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly)
    UMaterialInstance* RedMaterial;

    UPROPERTY(EditDefaultsOnly)
    UMaterialInstance* GreenMaterial;

    UPROPERTY(EditDefaultsOnly)
    UMaterialInstance* YellowMaterial;

    UPROPERTY(EditDefaultsOnly)
    UMaterialInstance* BlueMaterial;

    UPROPERTY()
    int32 StartCellIndex = -1;

    UPROPERTY()
    int32 FirstBoardCellIndex = -1;

    UPROPERTY()
    int32 CurrentCellIndex = -1;

    UPROPERTY()
    EPawnPosition CurrentPawnPosition = EPawnPosition::OnStart;
};
