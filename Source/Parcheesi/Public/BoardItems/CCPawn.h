// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CCCoreTypes.h"
#include "CCPawn.generated.h"

class USceneComponent;

UCLASS()
class PARCHEESI_API ACCPawn : public ACharacter
{
    GENERATED_BODY()

public:
    ACCPawn();

    void SetCurrentPawnPosition(EPawnPosition Position) { CurrentPawnPosition = Position; }
    EPawnPosition GetCurrentPawnPosition() { return CurrentPawnPosition; }

    int32 GetStartCellIndex() { return StartCellIndex; }
    int32 GetCurrentCellIndex() { return CurrentCellIndex; }
    int32 GetFirstBoardCellIndex() { return FirstBoardCellIndex; }
    int32 GetFirstFinishCellIndex() { return FirstFinishCellIndex; }
    bool GetIsInGates() { return bIsInGates; }

    void SetCurrentCellIndex(int32 Index) { CurrentCellIndex = Index; }
    void SetIsInGates(bool State) { bIsInGates = State; }

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_SetupPawnData(ETurnColors PawnColor, int32 StartCell, int32 FirstBoardCell, int32 FirstFinishCell);

    UPROPERTY(BlueprintReadOnly, Replicated)
    bool bIsAttacking = false;

    UPROPERTY(BlueprintReadOnly, Replicated)
    bool bIsInGates = false;

    UPROPERTY()
    USceneComponent* PawnPositonComponent;

protected:
    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

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
    int32 FirstFinishCellIndex = -1;

    UPROPERTY()
    int32 CurrentCellIndex = -1;


    UPROPERTY()
    EPawnPosition CurrentPawnPosition = EPawnPosition::OnStart;
};
