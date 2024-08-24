// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CCCell.generated.h"

UStaticMeshComponent;

UCLASS()
class PARCHEESI_API ACCCell : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACCCell();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
    UStaticMeshComponent* CellMeshComponent;
};
