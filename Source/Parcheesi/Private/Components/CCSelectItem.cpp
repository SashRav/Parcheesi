// Copyright Cats and Cubes, Inc. All Rights Reserved.


#include "Components/CCSelectItem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SceneComponent.h"
#include "Components/MeshComponent.h"
#include "Materials/Material.h"


UCCSelectItem::UCCSelectItem()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UCCSelectItem::BeginPlay()
{
	Super::BeginPlay();

}

void UCCSelectItem::SelectThisItem(UNiagaraSystem* NiagaraSystem, USceneComponent* ComponentToAdd, UMeshComponent* MeshToUpdate, UMaterial* OverlayMaterialToAdd)
{
	CurrentNiagaraSystem = UNiagaraFunctionLibrary::SpawnSystemAttached(NiagaraSystem,ComponentToAdd, "None", FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, true);
	MeshToUpdate->SetOverlayMaterial(OverlayMaterialToAdd);
}

void UCCSelectItem::DeselectThisItem(UMeshComponent* MeshToUpdate)
{
	if (CurrentNiagaraSystem) {
		CurrentNiagaraSystem->DestroyComponent();
		MeshToUpdate->SetOverlayMaterial(nullptr);
	}
}


