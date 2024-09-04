// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CCCoreTypes.h"
#include "CCPawn.generated.h"

class UStaticMeshComponent;

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
    void SetCurrentCellIndex(int32 Index) { CurrentCellIndex = Index; }


    UFUNCTION(NetMulticast, Reliable)
    void Multicast_SetupPawnData(ETurnColors PawnColor, int32 StartCell);

    UPROPERTY(EditDefaultsOnly)
    UStaticMeshComponent* PawnMeshComponent;

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
    int32 CurrentCellIndex = -1;

    UPROPERTY()
    EPawnPosition CurrentPawnPosition = EPawnPosition::OnStart;
};
