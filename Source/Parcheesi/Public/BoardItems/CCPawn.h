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

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_SetupPawnData(ETurnColors PawnColor);

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
};
