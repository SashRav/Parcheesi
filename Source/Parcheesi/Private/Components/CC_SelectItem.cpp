// Copyright Cats and Cubes, Inc. All Rights Reserved.


#include "Components/CC_SelectItem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SceneComponent.h"
#include "Components/MeshComponent.h"
#include "Materials/Material.h"


UCC_CSelectItem::UCC_CSelectItem()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UCC_CSelectItem::BeginPlay()
{
	Super::BeginPlay();

}

void UCC_CSelectItem::SelectThisItem(UNiagaraSystem* NiagaraSystem, USceneComponent* ComponentToAdd, UMeshComponent* MeshToUpdate, UMaterial* OverlayMaterialToAdd)
{
	CurrentNiagaraSystem = UNiagaraFunctionLibrary::SpawnSystemAttached(NiagaraSystem,ComponentToAdd, "None", FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, true);
	MeshToUpdate->SetOverlayMaterial(OverlayMaterialToAdd);
}

void UCC_CSelectItem::DeselectThisItem(UMeshComponent* MeshToUpdate)
{
	if (CurrentNiagaraSystem) {
		CurrentNiagaraSystem->DestroyComponent();
		MeshToUpdate->SetOverlayMaterial(nullptr);
	}
}


