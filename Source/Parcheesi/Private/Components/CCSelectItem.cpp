// Copyright Cats and Cubes, Inc. All Rights Reserved.


#include "Components/CCSelectItem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SceneComponent.h"
#include "Components/MeshComponent.h"
#include "Materials/Material.h"


UCCCSelectItem::UCCCSelectItem()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UCCCSelectItem::BeginPlay()
{
	Super::BeginPlay();

}

void UCCCSelectItem::SelectThisItem(UNiagaraSystem* NiagaraSystem, USceneComponent* ComponentToAdd, UMeshComponent* MeshToUpdate, UMaterial* OverlayMaterialToAdd)
{
	CurrentNiagaraSystem = UNiagaraFunctionLibrary::SpawnSystemAttached(NiagaraSystem,ComponentToAdd, "None", FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, true);
	MeshToUpdate->SetOverlayMaterial(OverlayMaterialToAdd);
}

void UCCCSelectItem::DeselectThisItem(UMeshComponent* MeshToUpdate)
{
	if (CurrentNiagaraSystem) {
		CurrentNiagaraSystem->DestroyComponent();
		MeshToUpdate->SetOverlayMaterial(nullptr);
	}
}


