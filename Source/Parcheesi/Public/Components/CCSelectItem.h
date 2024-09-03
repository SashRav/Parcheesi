// Copyright Cats and Cubes, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CCSelectItem.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;
class UMeshComponent;
class UMaterialInstance;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PARCHEESI_API UCCSelectItem : public UActorComponent
{
    GENERATED_BODY()

public:
    UCCSelectItem();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly)
    UMaterialInstance* OverlayMaterial;

    UPROPERTY(EditDefaultsOnly)
    UNiagaraSystem* OverlayNiagaraSystem;

public:
    UFUNCTION(BlueprintCallable, Category = "Select")
    void SelectThisItem(UMeshComponent* MeshToUpdate);

    UFUNCTION(BlueprintCallable, Category = "Select")
    void DeselectThisItem(UMeshComponent* MeshToUpdate);

private:
    UNiagaraComponent* CurrentNiagaraSystem;
};
