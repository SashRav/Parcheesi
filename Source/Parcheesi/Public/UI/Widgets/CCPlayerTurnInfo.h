// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CCPlayerTurnInfo.generated.h"

class UTextBlock;
class UImage;

UCLASS()
class PARCHEESI_API UCCPlayerTurnInfo : public UUserWidget
{
    GENERATED_BODY()

public:
    void SetPlayerInfoWidgetData(FString PlayerName, FLinearColor PlayerColor, bool TurnStatus);

protected:
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* T_PlayerName;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* T_TurnStatus;

    UPROPERTY(meta = (BindWidget))
    class UImage* I_PlayerColor;
};
