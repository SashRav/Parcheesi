// Copyright Cats and Cubes, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CCSelectItem.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;
class USceneComponent;
class UMeshComponent;
class UMaterial;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PARCHEESI_API UCCSelectItem : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCCSelectItem();

protected:
	virtual void BeginPlay() override;

public:	
	UFUNCTION(BlueprintCallable, Category = "Select")
	void SelectThisItem(UNiagaraSystem* NiagaraSystem, USceneComponent* ComponentToAdd, UMeshComponent* MeshToUpdate, UMaterial* OverlayMaterialToAdd);
	
	UFUNCTION(BlueprintCallable, Category="Select")
	void DeselectThisItem(UMeshComponent* MeshToUpdate);

private:
	UNiagaraComponent* CurrentNiagaraSystem;

};

