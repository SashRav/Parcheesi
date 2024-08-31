// Copyright Cats and Cubes. All Rights Reserved.

#include "Framework/CCPlayerPawnGame.h"
#include "Framework/CCGameModeBaseGame.h"
#include "Framework/CCGameStateGame.h"
#include "Framework/CCControllerGame.h"
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
    OwningPlayerController = Cast<ACCControllerGame>(GetController());

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
    if (OwningPlayerController)
    {
        FHitResult HitResult;
        if (OwningPlayerController->GetHitResultUnderCursor(ECC_Visibility, false, HitResult))
        {
            if (ACCDice* HitDice = Cast<ACCDice>(HitResult.GetActor()))
            {
                SelectedDiceActor = HitDice;
                UpdateSelectedDiceOnUI();
                UE_LOG(LogTemp, Log, TEXT("Hit Actor: %d"), HitDice->GetDiceSide());
            }
        }
    }
}

void ACCPlayerPawnGame::UpdateSelectedDiceOnUI()
{
    if (OwningPlayerController && SelectedDiceActor)
    {
        OwningPlayerController->Client_SetDiceSideOnUI(SelectedDiceActor->GetDiceSide());
    }
}

void ACCPlayerPawnGame::Server_RollDices_Implementation()
{

    FTimerHandle TimerHandle;
    FTimerDelegate TimerDelegate;

    FRotator Rotation;
    FVector SpawnLocation(500.0f, -100.0f, 600.0f);
    
    Rotation.Pitch = FMath::RandRange(-180.0, 180.0);
    Rotation.Yaw = FMath::RandRange(-180.0, 180.0);
    Rotation.Roll = FMath::RandRange(-180.0, 180.0);

    Server_SpawnDice(SpawnLocation, Rotation, true, true);

    Rotation.Pitch = FMath::RandRange(-180.0, 180.0);
    Rotation.Yaw = FMath::RandRange(-180.0, 180.0);
    Rotation.Roll = FMath::RandRange(-180.0, 180.0);
    SpawnLocation.Y = 100.0f;

    Server_SpawnDice(SpawnLocation, Rotation, true, true);

    TimerDelegate.BindUFunction(this, FName("MoveDicesToBoard"), DicePlacesLocation);
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, DiceMoveDelay, false);

    FTimerHandle DoubleTimerHandle;
    GetWorld()->GetTimerManager().SetTimer(DoubleTimerHandle, this, &ACCPlayerPawnGame::Server_TryDoubleDices, DiceMoveDelay + 1.0f, false);
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

void ACCPlayerPawnGame::Server_SpawnDice_Implementation(FVector SpawnOffset, FRotator Rotation, bool SpawnOnBoard, bool SimulatePhysics)
{
    ACCDice* SpawnedDice = GetWorld()->SpawnActor<ACCDice>(DiceClass, SpawnOffset, Rotation);
    ServerGameState->AddSpawnedDice(SpawnedDice);
    SpawnedDice->GetStaticMeshComponent()->SetSimulatePhysics(SimulatePhysics);
    if (SimulatePhysics)
        SetDiceVelocity(SpawnedDice);
    //SpawnDiceActor(SpawnOffset, Rotation, SpawnOnBoard, SimulatePhysics);
}

void ACCPlayerPawnGame::Server_SelectDiceSide_Implementation() {}

void ACCPlayerPawnGame::Server_CleanAllDices_Implementation()
{
    SelectedDiceActor = nullptr;
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
        Client_EnableTurnButton();
        return;
    }

    if (SpawnedDices[0]->GetDiceSide() == SpawnedDices[1]->GetDiceSide())
    {
        UE_LOG(LogTemp, Log, TEXT("Doubling dices!!!!"));

        FVector SpawnOffset(0.0f, 0.0f, -1000.0f);
        FRotator Rotation = *DiceSidesRotation.Find(SpawnedDices[0]->GetDiceSide());

        Server_SpawnDice(SpawnOffset, Rotation, false, false);
        SpawnOffset.X = 300.0f;
        Server_SpawnDice(SpawnOffset, Rotation, false, false);

        MoveDicesToBoard(DoubledDicePlacesLocation);
    }
    else
        UE_LOG(LogTemp, Log, TEXT("Dices are not equal"));
    Client_EnableTurnButton();
}

void ACCPlayerPawnGame::Client_EnableTurnButton_Implementation()
{
    OwningPlayerController->Client_EnableEndTurnButton();
}

void ACCPlayerPawnGame::SpawnDiceActor(FVector SpawnOffest, FRotator Rotation, bool UseVelocity, bool SimulatePhysics)
{
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
