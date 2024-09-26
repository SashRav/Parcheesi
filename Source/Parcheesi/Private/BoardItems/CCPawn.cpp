// Copyright Cats and Cubes. All Rights Reserved.

#include "BoardItems/CCPawn.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SplineComponent.h"

ACCPawn::ACCPawn()
{
    PrimaryActorTick.bCanEverTick = false;

    PawnSkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
    PawnSkeletalMeshComponent->SetupAttachment(RootComponent);

    SplienComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineMesh"));
    SplienComponent->SetupAttachment(RootComponent);
}

void ACCPawn::BeginPlay()
{
    Super::BeginPlay();

    check(RedMaterial);
    check(GreenMaterial);
    check(YellowMaterial);
    check(BlueMaterial);
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
        PawnSkeletalMeshComponent->SetMaterial(0, RedMaterial);
        break;
    case ETurnColors::Blue:
        PawnSkeletalMeshComponent->SetMaterial(0, BlueMaterial);
        break;
    case ETurnColors::Green:
        PawnSkeletalMeshComponent->SetMaterial(0, GreenMaterial);
        break;
    case ETurnColors::Yellow:
        PawnSkeletalMeshComponent->SetMaterial(0, YellowMaterial);
        break;
    }
}
