// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CCCoreTypes.h"
#include "CCPlayersTurnContainer.generated.h"

class UVerticalBox;
class UCCPlayerTurnInfo;

UCLASS()
class PARCHEESI_API UCCPlayersTurnContainer : public UUserWidget
{
	GENERATED_BODY()

public:
    void UpdatePlayersTurnData(const TArray<FPlayersTurnData>& PlayersTurnData);

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UCCPlayerTurnInfo> TurnInfoWidgetClass;

    UPROPERTY(meta = (BindWidget))
    class UVerticalBox* VB_Players;	
};
