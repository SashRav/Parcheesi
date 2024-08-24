// Copyright Cats and Cubes. All Rights Reserved.


#include "BoardItems/CCCell.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ACCCell::ACCCell()
{
	PrimaryActorTick.bCanEverTick = false;
    CellMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CellStaticMesh"));
    CellMeshComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ACCCell::BeginPlay()
{
	Super::BeginPlay();
	
}

