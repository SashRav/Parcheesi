// Copyright Cats and Cubes. All Rights Reserved.

#include "BoardItems/CCDice.h"
#include "Components/BoxComponent.h"

ACCDice::ACCDice()
{

    BoxSideFirst = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
    BoxSideFirst->SetupAttachment(RootComponent);
    BoxSideFirst->InitBoxExtent(FVector(32.0f, 32.0f, 32.0f));

    BoxSideSecond = CreateDefaultSubobject<UBoxComponent>(TEXT("Side2"));
    BoxSideSecond->SetupAttachment(RootComponent);
    BoxSideSecond->InitBoxExtent(FVector(32.0f, 32.0f, 32.0f));

    BoxSideThird = CreateDefaultSubobject<UBoxComponent>(TEXT("Side3"));
    BoxSideThird->SetupAttachment(RootComponent);
    BoxSideThird->InitBoxExtent(FVector(32.0f, 32.0f, 32.0f));

    BoxSideFourth = CreateDefaultSubobject<UBoxComponent>(TEXT("Side4"));
    BoxSideFourth->SetupAttachment(RootComponent);
    BoxSideFourth->InitBoxExtent(FVector(32.0f, 32.0f, 32.0f));

    BoxSideFifth = CreateDefaultSubobject<UBoxComponent>(TEXT("Side5"));
    BoxSideFifth->SetupAttachment(RootComponent);
    BoxSideFifth->InitBoxExtent(FVector(32.0f, 32.0f, 32.0f));

    BoxSideFSixth = CreateDefaultSubobject<UBoxComponent>(TEXT("Side6"));
    BoxSideFSixth->SetupAttachment(RootComponent);
    BoxSideFSixth->InitBoxExtent(FVector(32.0f, 32.0f, 32.0f));
}

int32 ACCDice::GetDiceSide()
{
    int32 DiceSide = 0;
    double TopComponentHeight = -100000.0;
    UBoxComponent* TopComponent = nullptr;

    TArray<UBoxComponent*> BoxComponents;
    GetComponents(BoxComponents);

    for (UBoxComponent* Component : BoxComponents)
    {
        if (TopComponentHeight < Component->GetComponentTransform().GetLocation().Z)
        {
            TopComponentHeight = Component->GetComponentTransform().GetLocation().Z;
            TopComponent = Component;
        }
    }
    if (TopComponent)
    {
        UE_LOG(LogTemp, Log, TEXT("Top Component Exists"));
        if (TopComponent == BoxSideFirst)
            DiceSide = 1;
        else if (TopComponent == BoxSideSecond)
            DiceSide = 2;
        else if (TopComponent == BoxSideThird)
            DiceSide = 3;
        else if (TopComponent == BoxSideFourth)
            DiceSide = 4;
        else if (TopComponent == BoxSideFifth)
            DiceSide = 5;
        else if (TopComponent == BoxSideFSixth)
            DiceSide = 6;
    }

    return DiceSide;
}

void ACCDice::GetHighestComponent() {}
