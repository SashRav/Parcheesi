// Copyright Cats and Cubes, Inc. All Rights Reserved.

#include "Components/CCSelectItem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/MeshComponent.h"
#include "Materials/Material.h"

UCCSelectItem::UCCSelectItem()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UCCSelectItem::BeginPlay()
{
    Super::BeginPlay();
}

void UCCSelectItem::SelectThisItem(UMeshComponent* MeshToUpdate)
{
    CurrentNiagaraSystem = UNiagaraFunctionLibrary::SpawnSystemAttached(
        OverlayNiagaraSystem, MeshToUpdate, "None", FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, true);
    MeshToUpdate->SetOverlayMaterial(OverlayMaterial);
}

void UCCSelectItem::DeselectThisItem(UMeshComponent* MeshToUpdate)
{
    if (CurrentNiagaraSystem)
    {
        CurrentNiagaraSystem->DestroyComponent();
        MeshToUpdate->SetOverlayMaterial(nullptr);
    }
}
