// Copyright Cats and Cubes. All Rights Reserved.

#include "Components/CCDiceComponent.h"
#include "BoardItems/CCDice.h"
#include "Framework/CCGameStateGame.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

void UCCDiceComponent::BeginPlay()
{
    Super::BeginPlay();

    ServerGameState = Cast<ACCGameStateGame>(UGameplayStatics::GetGameState(GetWorld()));

    TArray<AActor*> FoundDicePlaces;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), DicePlaceTag, FoundDicePlaces);

    if (FoundDicePlaces.Num() == 0)
        return;

    for (AActor* PlaceActor : FoundDicePlaces)
    {
        DicePlacesLocation.Add(PlaceActor->GetActorLocation());
    }

    TArray<AActor*> FoundDoubledDicePlaces;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), DoubledDicePlaceTag, FoundDoubledDicePlaces);

    if (FoundDoubledDicePlaces.Num() == 0)
        return;

    for (AActor* PlaceActor : FoundDoubledDicePlaces)
    {
        DoubledDicePlacesLocation.Add(PlaceActor->GetActorLocation());
    }
}

void UCCDiceComponent::RollDices(FVector ForwardVector)
{
    if (!ServerGameState || !PlayerSpringArm)
        return;

    ForwardVectorToSpawn = ForwardVector;

    FTimerHandle TimerHandle;
    FTimerDelegate TimerDelegate;

    FRotator Rotation = PlayerSpringArm->GetComponentRotation();
    FVector SpawnLocation(500.0f, -100.0f, 600.0f);
    int32 DicesToSpawn = ServerGameState->GetGameSettings().DicesToRool;

    if (DicesToSpawn < 1 || DicesToSpawn > 4) // Additional check to not spawn 0 dices or more then places available
        return;

    for (int32 DiceNumber = 1; DiceNumber <= DicesToSpawn; DiceNumber++)
    {
        SpawnLocation.Y = FMath::RandRange(-100.0, 100.0) * DiceNumber;
        SpawnLocation.X = FMath::RandRange(-100.0, 100.0) * DiceNumber;

        SpawnDice(SpawnLocation, Rotation, true, GetOwner()->HasAuthority()); // enable physic simulation only on server
    }

    TimerDelegate.BindUFunction(this, FName("MoveDicesToBoard"), DicePlacesLocation);
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, DiceMoveDelay, false);

    FTimerHandle DoubleTimerHandle;
    GetWorld()->GetTimerManager().SetTimer(DoubleTimerHandle, this, &UCCDiceComponent::TryDoubleDices, DiceMoveDelay + 1.0f, false);
}

void UCCDiceComponent::SpawnDice(FVector SpawnLocation, FRotator Rotation, bool UseVelocity, bool SimulatePhysics)
{
    if (!ServerGameState)
        return;

    ACCDice* SpawnedDice = GetWorld()->SpawnActor<ACCDice>(DiceClass, SpawnLocation, Rotation);
    if (SpawnedDice)
    {
        ServerGameState->AddSpawnedDice(SpawnedDice);
        SpawnedDice->GetStaticMeshComponent()->SetSimulatePhysics(SimulatePhysics);
        if (UseVelocity)
            SetDiceVelocity(SpawnedDice);
    }
}

void UCCDiceComponent::SetDiceVelocity(ACCDice* Dice)
{
    if (!Dice || !PlayerSpringArm)
        return;

    FVector DiceVelocity = ForwardVectorToSpawn;
    UE_LOG(LogTemp, Display, TEXT("Init Vector: %s"), *DiceVelocity.ToString());

    DiceVelocity.X = DiceVelocity.X * 6000;
    DiceVelocity.Y = DiceVelocity.Y * 6000;
    DiceVelocity.Z = FMath::RandRange(-1200.0, -500.0);
    UE_LOG(LogTemp, Display, TEXT("New Vector: %s"), *DiceVelocity.ToString());
    
    Dice->GetStaticMeshComponent()->SetPhysicsLinearVelocity(DiceVelocity);
}

void UCCDiceComponent::TryDoubleDices()
{
    TArray<ACCDice*> SpawnedDices = ServerGameState->GetSpawnedDices();
    UE_LOG(LogTemp, Log, TEXT("Trying to double dices. Dices number: %d"), SpawnedDices.Num());

    if (SpawnedDices.Num() != 2)
    {
        UE_LOG(LogTemp, Log, TEXT("Dices count is not 2"));
        OnDiceRollingEnd.Broadcast();
        return;
    }

    if (SpawnedDices[0]->GetDiceSide() == SpawnedDices[1]->GetDiceSide())
    {
        UE_LOG(LogTemp, Log, TEXT("Doubling dices!!!!"));

        FVector SpawnOffset(0.0f, 0.0f, -1000.0f);
        FRotator Rotation = *DiceSidesRotation.Find(SpawnedDices[0]->GetDiceSide());

        SpawnDice(SpawnOffset, Rotation, false, false);
        SpawnOffset.X = 300.0f;
        SpawnDice(SpawnOffset, Rotation, false, false);

        MoveDicesToBoard(DoubledDicePlacesLocation);
    }
    else
        UE_LOG(LogTemp, Log, TEXT("Dices are not equal"));
    OnDiceRollingEnd.Broadcast();
}

void UCCDiceComponent::MoveDicesToBoard(TArray<FVector> TargetLocations)
{
    if (!ServerGameState)
        return;

    TArray<ACCDice*> SpawnedDices = ServerGameState->GetSpawnedDices();

    if (SpawnedDices.Num() == 0)
        return;

    for (int Index = 0; Index < SpawnedDices.Num(); Index++)
    {
        if (TargetLocations.IsValidIndex(Index))
        {
            FRotator DiceRotation = *DiceSidesRotation.Find(SpawnedDices[Index]->GetDiceSide());
            SpawnedDices[Index]->SetActorLocationAndRotation(TargetLocations[Index], DiceRotation);
            SpawnedDices[Index]->GetStaticMeshComponent()->SetSimulatePhysics(false);
        }
    }
}
