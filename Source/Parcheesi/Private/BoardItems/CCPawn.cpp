// Copyright Cats and Cubes. All Rights Reserved.

#include "BoardItems/CCPawn.h"
#include "AI/CCPawnAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

ACCPawn::ACCPawn()
{
    PrimaryActorTick.bCanEverTick = true;

    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
    AIControllerClass = ACCPawnAIController::StaticClass();

    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->bRequestedMoveUseAcceleration = false;

    PawnPositonComponent = CreateDefaultSubobject<USceneComponent>(TEXT("PawnPositonComponent"));
    PawnPositonComponent->SetupAttachment(RootComponent);
    PawnPositonComponent->SetUsingAbsoluteRotation(true);
}

void ACCPawn::BeginPlay()
{
    Super::BeginPlay();

    check(RedMaterial);
    check(GreenMaterial);
    check(YellowMaterial);
    check(BlueMaterial);

    GetMovementComponent()->SetComponentTickEnabled(false);
}

void ACCPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ACCPawn, bIsAttacking);
    DOREPLIFETIME(ACCPawn, bIsInGates);
}

void ACCPawn::Multicast_SetupPawnData_Implementation(ETurnColors PawnColor, int32 StartCell, int32 FirstBoardCell, int32 FirstFinishCell)
{
    Tags.Add(UEnum::GetValueAsName(PawnColor));

    StartCellIndex = StartCell;
    FirstBoardCellIndex = FirstBoardCell;
    FirstFinishCellIndex = FirstFinishCell;

    switch (PawnColor)
    {
    case ETurnColors::Red:
        GetMesh()->SetMaterial(0, RedMaterial);
        break;
    case ETurnColors::Blue:
        GetMesh()->SetMaterial(0, BlueMaterial);
        break;
    case ETurnColors::Green:
        GetMesh()->SetMaterial(0, GreenMaterial);
        break;
    case ETurnColors::Yellow:
        GetMesh()->SetMaterial(0, YellowMaterial);
        break;
    }
}

void ACCPawn::SetMovementTick_Implementation(bool State)
{
    GetMesh()->bComponentUseFixedSkelBounds = State;
    GetMovementComponent()->SetComponentTickEnabled(State);
}