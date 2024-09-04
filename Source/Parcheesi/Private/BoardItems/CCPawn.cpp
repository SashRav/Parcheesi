// Copyright Cats and Cubes. All Rights Reserved.

#include "BoardItems/CCPawn.h"
#include "Components/StaticMeshComponent.h"

ACCPawn::ACCPawn()
{
    PrimaryActorTick.bCanEverTick = false;

    PawnMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pawn Mesh"));
    PawnMeshComponent->SetupAttachment(RootComponent);
}

void ACCPawn::BeginPlay()
{
    Super::BeginPlay();

    check(RedMaterial);
    check(GreenMaterial);
    check(YellowMaterial);
    check(BlueMaterial);
}

void ACCPawn::Multicast_SetupPawnData_Implementation(ETurnColors PawnColor, int32 StartCell)
{
    Tags.Add(UEnum::GetValueAsName(PawnColor));

    StartCellIndex = StartCell;

    switch (PawnColor)
    {
    case ETurnColors::Red:
        PawnMeshComponent->SetMaterial(0, RedMaterial);
        break;
    case ETurnColors::Blue:
        PawnMeshComponent->SetMaterial(0, BlueMaterial);
        break;
    case ETurnColors::Green:
        PawnMeshComponent->SetMaterial(0, GreenMaterial);
        break;
    case ETurnColors::Yellow:
        PawnMeshComponent->SetMaterial(0, YellowMaterial);
        break;
    }
}
