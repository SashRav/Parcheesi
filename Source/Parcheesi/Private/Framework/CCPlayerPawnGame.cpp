// Copyright Cats and Cubes. All Rights Reserved.

#include "Framework/CCPlayerPawnGame.h"
#include "Framework/CCGameModeBaseGame.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerState.h"

ACCPlayerPawnGame::ACCPlayerPawnGame() {}

void ACCPlayerPawnGame::BeginPlay()
{
    Super::BeginPlay();

    ServerGameMode = Cast<ACCGameModeBaseGame>(UGameplayStatics::GetGameMode(GetWorld()));
}

void ACCPlayerPawnGame::Server_UpdateSelectedColor_Implementation(const FName& ColorTag)
{
    UE_LOG(LogTemp, Display, TEXT("Color Tag clicked: %s"), *ColorTag.ToString());

    ServerGameMode->ChangePlayerTag(GetPlayerState()->GetUniqueId(), ColorTag);
}

void ACCPlayerPawnGame::Server_EndPlayerTurn_Implementation()
{
    UE_LOG(LogTemp, Display, TEXT("End turn from Pawn"));
    ServerGameMode->StartNextTurn();
}

void ACCPlayerPawnGame::Server_DebugEndPlayerTurn_Implementation()
{
    UE_LOG(LogTemp, Display, TEXT("Debug End turn from Pawn"));
    ServerGameMode->StartNextTurn();
}
