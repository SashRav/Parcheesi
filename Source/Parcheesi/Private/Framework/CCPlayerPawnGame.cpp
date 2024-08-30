// Copyright Cats and Cubes. All Rights Reserved.

#include "Framework/CCPlayerPawnGame.h"
#include "Framework/CCGameModeBaseGame.h"
#include "Framework/CCGameStateGame.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerController.h"
#include "BoardItems/CCDice.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"

ACCPlayerPawnGame::ACCPlayerPawnGame() {}

void ACCPlayerPawnGame::BeginPlay()
{
    Super::BeginPlay();

    check(DiceClass);
    check(ClickOnBoardAction);

    ServerGameMode = Cast<ACCGameModeBaseGame>(UGameplayStatics::GetGameMode(GetWorld()));
    ServerGameState = Cast<ACCGameStateGame>(UGameplayStatics::GetGameState(GetWorld()));

    TArray<AActor*> FoundDicePlaces;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), DicePlaceTag, FoundDicePlaces);

    for (AActor* PlaceActor : FoundDicePlaces)
    {
        DicePlacesLocation.Add(PlaceActor->GetActorLocation());
    }

    TArray<AActor*> FoundDoubledDicePlaces;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), DoubledDicePlaceTag, FoundDoubledDicePlaces);

    for (AActor* PlaceActor : FoundDoubledDicePlaces)
    {
        DoubledDicePlacesLocation.Add(PlaceActor->GetActorLocation());
        UE_LOG(LogTemp, Display, TEXT("Doubled dice place added"));
    }
}

void ACCPlayerPawnGame::SetupPlayerInputComponent(UInputComponent* NewInputComponent)
{
    Super::SetupPlayerInputComponent(InputComponent);
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
    {
        EnhancedInputComponent->BindAction(ClickOnBoardAction, ETriggerEvent::Started, this, &ACCPlayerPawnGame::ClickOnBoard);
    }
}

void ACCPlayerPawnGame::ClickOnBoard()
{
    UE_LOG(LogTemp, Log, TEXT("Click action hit"));
    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    if (PlayerController)
    {
        FHitResult HitResult;
        if (PlayerController->GetHitResultUnderCursor(ECC_Visibility, false, HitResult))
        {
            if (ACCDice* HitDice = Cast<ACCDice>(HitResult.GetActor()))
            {
                UE_LOG(LogTemp, Log, TEXT("Hit Actor: %d"), HitDice->GetDiceSide());
            }
        }
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

void ACCPlayerPawnGame::Server_SpawnDice_Implementation(bool SpawnOnBoard, bool SimulatePhysics, int32 DicesToSpawn)
{

    /// Update logic
    if (SpawnOnBoard)
    {
        FVector SpawnOffset(500.0f, 0.0f, 600.0f);
        for (int i = 0; i < DicesToSpawn; i++)
        {
            SpawnOffset.Y = -100.0f * i;
            SpawnDiceActor(SpawnOffset, SpawnOnBoard, SimulatePhysics);
        }
    }
    else
    {
        FVector SpawnOffset(0.0f, 0.0f, -4000.0f);
        for (int i = 0; i < DicesToSpawn; i++)
        {
            SpawnOffset.Y = -300.0f * i;
            SpawnDiceActor(SpawnOffset, SpawnOnBoard, SimulatePhysics);
        }
    }

    FTimerHandle TimerHandle;
    FTimerDelegate TimerDelegate;

    TimerDelegate.BindUFunction(this, FName("MoveDicesToBoard"), DicePlacesLocation);
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, DiceMoveDelay, false);

    FTimerHandle DoubleTimerHandle;
    GetWorld()->GetTimerManager().SetTimer(DoubleTimerHandle, this, &ACCPlayerPawnGame::Server_TryDoubleDices, DiceMoveDelay + 1.0f, false);
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

void ACCPlayerPawnGame::Server_TryDoubleDices_Implementation()
{
    TArray<ACCDice*> SpawnedDices = ServerGameState->GetSpawnedDices();
    UE_LOG(LogTemp, Log, TEXT("Trying to double dices. Dices number: %d"), SpawnedDices.Num());

    if (SpawnedDices.Num() != 2)
    {
        UE_LOG(LogTemp, Log, TEXT("Dices count is not 2"));
        return;
    }

    if (SpawnedDices[0]->GetDiceSide() == SpawnedDices[1]->GetDiceSide())
    {
        UE_LOG(LogTemp, Log, TEXT("Doubling dices!!!!"));
        FVector SpawnOffset(0.0f, 0.0f, -1000.0f);

        Server_SpawnDice(false, false, 2);
        MoveDicesToBoard(DoubledDicePlacesLocation);
    }
    else
        UE_LOG(LogTemp, Log, TEXT("Dices are not equal"));
}

void ACCPlayerPawnGame::SpawnDiceActor(FVector SpawnOffest, bool UseVelocity, bool SimulatePhysics)
{
    FRotator Rotation(-45.0f, 30.0f, -30.0f);
    ACCDice* SpawnedDice = GetWorld()->SpawnActor<ACCDice>(DiceClass, SpawnOffest, Rotation);
    ServerGameState->AddSpawnedDice(SpawnedDice);
    SpawnedDice->GetStaticMeshComponent()->SetSimulatePhysics(SimulatePhysics);
    if (UseVelocity)
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

void ACCPlayerPawnGame::MoveDicesToBoard(TArray<FVector> TargetLocations)
{
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
