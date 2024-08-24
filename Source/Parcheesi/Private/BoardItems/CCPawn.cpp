// Copyright Cats and Cubes. All Rights Reserved.


#include "BoardItems/CCPawn.h"

// Sets default values
ACCPawn::ACCPawn()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACCPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACCPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

