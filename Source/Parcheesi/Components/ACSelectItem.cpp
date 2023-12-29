// Copyright Cats and Cubes, Inc. All Rights Reserved.


#include "ACSelectItem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SceneComponent.h"
#include "Components/MeshComponent.h"
#include "Materials/Material.h"


UACSelectItem::UACSelectItem()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UACSelectItem::BeginPlay()
{
	Super::BeginPlay();

}

void UACSelectItem::SelectThisItem(UNiagaraSystem* NiagaraSystem, USceneComponent* ComponentToAdd, UMeshComponent* MeshToUpdate, UMaterial* OverlayMaterialToAdd)
{
	CurrentNiagaraSystem = UNiagaraFunctionLibrary::SpawnSystemAttached(NiagaraSystem,ComponentToAdd, "None", FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, true);
	MeshToUpdate->SetOverlayMaterial(OverlayMaterialToAdd);
}

void UACSelectItem::DeselectThisItem(UMeshComponent* MeshToUpdate)
{
	if (CurrentNiagaraSystem) {
		CurrentNiagaraSystem->DestroyComponent();
		MeshToUpdate->SetOverlayMaterial(nullptr);
	}
}


