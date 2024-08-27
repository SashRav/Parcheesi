// Copyright Cats and Cubes. All Rights Reserved.

#include "Framework/CCPlayerPawnGame.h"
#include "Framework/CCGameModeBaseGame.h"
#include "Framework/CCGameStateGame.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerState.h"
#include "BoardItems/CCDice.h"

ACCPlayerPawnGame::ACCPlayerPawnGame() {}

void ACCPlayerPawnGame::BeginPlay()
{
    Super::BeginPlay();
    check(DiceClass);
    ServerGameMode = Cast<ACCGameModeBaseGame>(UGameplayStatics::GetGameMode(GetWorld()));
    ServerGameState = Cast<ACCGameStateGame>(UGameplayStatics::GetGameState(GetWorld()));

    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), DicePlaceTag, FoundActors);

    for (AActor* PlaceActor : FoundActors)
    {
        DicePlacesLocation.Add(PlaceActor->GetActorLocation());
    }
}

void ACCPlayerPawnGame::Server_UpdateSelectedColor_Implementation(const FName& ColorTag)
{
    UE_LOG(LogTemp, Display, TEXT("Color Tag clicked: %s"), *ColorTag.ToString());

    ServerGameMode->ChangePlayerTag(GetPlayerState()->GetUniqueId(), ColorTag);
}

void ACCPlayerPawnGame::Server_EndPlayerTurn_Implementation()
{
    UE_LOG(LogTemp, Display, TEXT("End turn from Pawn"));
    Server_CleanAllDices();
    ServerGameMode->StartNextTurn();
}

void ACCPlayerPawnGame::Server_DebugEndPlayerTurn_Implementation()
{
    UE_LOG(LogTemp, Display, TEXT("Debug End turn from Pawn"));
    Server_EndPlayerTurn();
}

void ACCPlayerPawnGame::Server_SpawnDice_Implementation()
{
    FVector SpawnOffset(500.0f, 0.0f, 600.0f);
    // Hardcoded for now
    SpawnOffset.Y = 200.0f;
    SpawnDiceActor(SpawnOffset);
    SpawnOffset.Y = -200.0f;
    SpawnDiceActor(SpawnOffset);

    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ACCPlayerPawnGame::MoveDicesToBoard, DiceMoveDelay, false);
}

void ACCPlayerPawnGame::Server_SelectDiceSide_Implementation() {}

void ACCPlayerPawnGame::Server_CleanAllDices_Implementation()
{
    for (ACCDice* Dice : ServerGameState->GetSpawnedDices())
    {
        ServerGameState->RemoveDice(Dice);
        Dice->Destroy();
    }
    // Additional cleaning up for array
    ServerGameState->CleanSpawnedDicesArray();
}

void ACCPlayerPawnGame::Server_MoveSelectedPawn_Implementation() {}

void ACCPlayerPawnGame::SpawnDiceActor(FVector SpawnOffest)
{
    FRotator Rotation(-45.0f, 30.0f, -30.0f);
    ACCDice* SpawnedDice = GetWorld()->SpawnActor<ACCDice>(DiceClass, SpawnOffest, Rotation);
    ServerGameState->AddSpawnedDice(SpawnedDice);
    SetDiceVelocity(SpawnedDice);
}

void ACCPlayerPawnGame::SetDiceVelocity(ACCDice* Dice)
{
    FVector DiceVelocity(0.0f, 0.0f, 0.0f);
    // Hardcoded for now
    DiceVelocity.X = FMath::RandRange(-1750.0, -1250.0);
    DiceVelocity.Y = FMath::RandRange(-200.0, 200.0);
    DiceVelocity.Z = FMath::RandRange(-1200.0, -500.0);
    Dice->GetStaticMeshComponent()->SetPhysicsLinearVelocity(DiceVelocity);
}

void ACCPlayerPawnGame::MoveDicesToBoard() {
    TArray<ACCDice*> SpawnedDices =  ServerGameState->GetSpawnedDices();

    if (SpawnedDices.Num() == 0)
        return;

    for (int Index = 0; Index < SpawnedDices.Num(); Index++)
    {
        if (DicePlacesLocation.IsValidIndex(Index))
        {
            FRotator DiceRotation;
            SpawnedDices[Index]->SetActorLocationAndRotation(DicePlacesLocation[Index], DiceRotation);
        }
    }

}

void ACCPlayerPawnGame::TryDoubleDices() {}
