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
#include "Components/CCPawnManagerComponent.h"
#include "Net/UnrealNetwork.h"

ACCPlayerPawnGame::ACCPlayerPawnGame()
{
    DiceComponent = CreateDefaultSubobject<UCCDiceComponent>(TEXT("DiceComponent"));
    SelectItemDiceComponent = CreateDefaultSubobject<UCCSelectItem>(TEXT("SelectionDiceComponent"));
    SelectItemPawnComponent = CreateDefaultSubobject<UCCSelectItem>(TEXT("SelectionPawnComponent"));
    PawnManagerComponent = CreateDefaultSubobject<UCCPawnManagerComponent>(TEXT("PawnManagerComponent"));
}

void ACCPlayerPawnGame::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ACCPlayerPawnGame, PlayerTagName);
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
    Server_CleanSelectionData();
    Multicast_SetCurrentTurn(false);
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

void ACCPlayerPawnGame::Server_CleanSelectionData_Implementation()
{
    if (SelectedDiceActor)
    {
        Client_VisualDeselectActor(SelectItemDiceComponent, SelectedDiceActor->GetStaticMeshComponent());
        SelectedDiceActor = nullptr;
    }
    if (SelectedPawnActor)
    {
        Client_VisualDeselectActor(SelectItemPawnComponent, SelectedPawnActor->PawnMeshComponent);
        SelectedPawnActor = nullptr;
    }
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
    // Needed to get Player controller because it does not exist in the context
    ACCControllerGame* PlayerController = Cast<ACCControllerGame>(GetController());
    if (!PlayerController)
    {
        UE_LOG(LogTemp, Log, TEXT("OwningPlayerController is not valid in UpdateSelectedDiceOnUI"));
        return;
    }
    if (!SelectedDiceActor)
    {
        UE_LOG(LogTemp, Log, TEXT("SelectedDiceActor is not valid"));
        return;
    }
    PlayerController->Client_SetDiceSideOnUI(SelectedDiceActor->GetDiceSide());
}

void ACCPlayerPawnGame::Server_SwitchMovePawnButtonIsEnabled_Implementation(bool State)
{
    // Needed to get Player controller because it does not exist in the context
    ACCControllerGame* PlayerController = Cast<ACCControllerGame>(GetController());
    if (!PlayerController)
    {
        UE_LOG(LogTemp, Log, TEXT("OwningPlayerController is not valid in SwitchMovePawnButtonIsEnabled"));
        return;
    }
    if (SelectedDiceActor && SelectedPawnActor)
        PlayerController->Client_SwitchMovePawnButtonIsEnabled(true);
}

void ACCPlayerPawnGame::Client_VisualDeselectActor_Implementation(UCCSelectItem* Component, UMeshComponent* Mesh)
{
    Component->DeselectThisItem(Mesh);
}

void ACCPlayerPawnGame::Client_VisualSelectActor_Implementation(UCCSelectItem* Component, UMeshComponent* Mesh)
{
    Component->SelectThisItem(Mesh);
}

void ACCPlayerPawnGame::Server_SelectDiceActor_Implementation(ACCDice* HitDice)
{
    if (SelectedDiceActor)
        Client_VisualDeselectActor(SelectItemDiceComponent, SelectedDiceActor->GetStaticMeshComponent());

    SelectedDiceActor = HitDice;
    Client_VisualSelectActor(SelectItemDiceComponent, SelectedDiceActor->GetStaticMeshComponent());

    UpdateSelectedDiceOnUI();
}

void ACCPlayerPawnGame::Server_SelectPawnActor_Implementation(ACCPawn* HitPawn)
{
    if (HitPawn->Tags[0] != PlayerTagName)
        return;

    if (SelectedPawnActor)
        Client_VisualDeselectActor(SelectItemPawnComponent, SelectedPawnActor->PawnMeshComponent);

    SelectedPawnActor = HitPawn;
    Client_VisualSelectActor(SelectItemPawnComponent, SelectedPawnActor->PawnMeshComponent);

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
                Server_SelectDiceActor(HitDice);
            }
            else if (ACCPawn* HitPawn = Cast<ACCPawn>(HitResult.GetActor()))
            {
                Server_SelectPawnActor(HitPawn);
            }
        }
        Server_SwitchMovePawnButtonIsEnabled(true);
    }
}

void ACCPlayerPawnGame::Server_MoveSelectedPawn_Implementation()
{
    UE_LOG(LogTemp, Log, TEXT("Moving Pawn"));

    if (!SelectedDiceActor || !SelectedPawnActor)
        return;

    PawnManagerComponent->MoveSelectedPawn(SelectedPawnActor, SelectedDiceActor->GetDiceSide());

    Client_VisualDeselectActor(SelectItemPawnComponent, SelectedPawnActor->PawnMeshComponent);

    ServerGameState->RemoveDice(SelectedDiceActor);//Exists for debug purpose
    if (SelectedDiceActor->bDestryWhenUsed)
    {
        SelectedDiceActor->Destroy();
    }

    SelectedPawnActor = nullptr;
    SelectedDiceActor = nullptr;
}