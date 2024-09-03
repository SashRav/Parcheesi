// Copyright Cats and Cubes. All Rights Reserved.

#include "Framework/CCPlayerPawnGame.h"
#include "Framework/CCGameModeBaseGame.h"
#include "Framework/CCGameStateGame.h"
#include "Framework/CCControllerGame.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerController.h"
#include "BoardItems/CCDice.h"
#include "BoardItems/CCPawn.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "Components/CCDiceComponent.h"
#include "Components/CCSelectItem.h"

ACCPlayerPawnGame::ACCPlayerPawnGame()
{
    DiceComponent = CreateDefaultSubobject<UCCDiceComponent>(TEXT("DiceComponent"));
    SelectItemDiceComponent = CreateDefaultSubobject<UCCSelectItem>(TEXT("SelectionDiceComponent"));
    SelectItemPawnComponent = CreateDefaultSubobject<UCCSelectItem>(TEXT("SelectionPawnComponent"));
}

void ACCPlayerPawnGame::BeginPlay()
{
    Super::BeginPlay();

    check(ClickOnBoardAction);

    ServerGameMode = Cast<ACCGameModeBaseGame>(UGameplayStatics::GetGameMode(GetWorld()));
    ServerGameState = Cast<ACCGameStateGame>(UGameplayStatics::GetGameState(GetWorld()));
    OwningPlayerController = Cast<ACCControllerGame>(GetController());

    DiceComponent->OnDiceRollingEnd.AddDynamic(this, &ACCPlayerPawnGame::Client_EnableTurnButton);
}

void ACCPlayerPawnGame::SetupPlayerInputComponent(UInputComponent* NewInputComponent)
{
    Super::SetupPlayerInputComponent(InputComponent);
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
    {
        EnhancedInputComponent->BindAction(ClickOnBoardAction, ETriggerEvent::Started, this, &ACCPlayerPawnGame::ClickOnBoard);
    }
}

void ACCPlayerPawnGame::Server_RollDices_Implementation()
{
    DiceComponent->RollDices();
}

void ACCPlayerPawnGame::Server_UpdateSelectedColor_Implementation(const FName& ColorTag)
{
    UE_LOG(LogTemp, Display, TEXT("Color Tag clicked: %s"), *ColorTag.ToString());

    ServerGameMode->ChangePlayerTag(GetPlayerState()->GetUniqueId(), ColorTag);
}

void ACCPlayerPawnGame::Server_EndPlayerTurn_Implementation()
{
    UE_LOG(LogTemp, Display, TEXT("End turn from Pawn"));
    Multicast_CleanTurnData();
    Server_CleanAllDices();
    ServerGameMode->StartNextTurn();
}

void ACCPlayerPawnGame::Server_DebugEndPlayerTurn_Implementation()
{
    UE_LOG(LogTemp, Display, TEXT("Debug End turn from Pawn"));
    Server_EndPlayerTurn();
}

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

void ACCPlayerPawnGame::Multicast_CleanTurnData_Implementation()
{
    if (SelectedDiceActor)
    {
        SelectItemDiceComponent->DeselectThisItem(SelectedDiceActor->GetStaticMeshComponent());
    }

    if (SelectedPawnActor)
    {
        SelectItemPawnComponent->DeselectThisItem(SelectedPawnActor->PawnMeshComponent);
    }

    Multicast_SetCurrentTurn(false);
}

void ACCPlayerPawnGame::Multicast_SetCurrentTurn_Implementation(bool Turn)
{
    bCurrentTurn = Turn;
}

void ACCPlayerPawnGame::Client_EnableTurnButton_Implementation()
{
    OwningPlayerController->Client_EnableEndTurnButton();
}

void ACCPlayerPawnGame::UpdateSelectedDiceOnUI()
{
    if (OwningPlayerController && SelectedDiceActor)
    {
        OwningPlayerController->Client_SetDiceSideOnUI(SelectedDiceActor->GetDiceSide());
    }
}

void ACCPlayerPawnGame::SelectDiceActor(ACCDice* HitDice)
{
    if (SelectedDiceActor)
        SelectItemDiceComponent->DeselectThisItem(SelectedDiceActor->GetStaticMeshComponent());

    SelectedDiceActor = HitDice;
    SelectItemDiceComponent->SelectThisItem(SelectedDiceActor->GetStaticMeshComponent());

    UpdateSelectedDiceOnUI();

    UE_LOG(LogTemp, Log, TEXT("Dice Actor Selected"));
}

void ACCPlayerPawnGame::SelectPawnActor(ACCPawn* HitPawn)
{
    if (SelectedPawnActor)
        SelectItemPawnComponent->DeselectThisItem(SelectedPawnActor->PawnMeshComponent);

    SelectedPawnActor = HitPawn;
    SelectItemPawnComponent->SelectThisItem(SelectedPawnActor->PawnMeshComponent);

    UE_LOG(LogTemp, Log, TEXT("Pawn Actor Selected"));
}

void ACCPlayerPawnGame::ClickOnBoard()
{
    UE_LOG(LogTemp, Log, TEXT("Click action hit"));
    if (!bCurrentTurn)
        return;

    if (OwningPlayerController)
    {
        FHitResult HitResult;
        if (OwningPlayerController->GetHitResultUnderCursor(ECC_Visibility, false, HitResult))
        {
            if (ACCDice* HitDice = Cast<ACCDice>(HitResult.GetActor()))
            {
                SelectDiceActor(HitDice);
            }
            else if (ACCPawn* HitPawn = Cast<ACCPawn>(HitResult.GetActor()))
            {
                SelectPawnActor(HitPawn);
            }
        }
    }
}

void ACCPlayerPawnGame::Server_MoveSelectedPawn_Implementation() {}