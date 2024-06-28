// Copyright Cats and Cubes. All Rights Reserved.


#include "Framework/CCPlayerPawnGame.h"

// Sets default values
ACCPlayerPawnGame::ACCPlayerPawnGame()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACCPlayerPawnGame::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACCPlayerPawnGame::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACCPlayerPawnGame::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

