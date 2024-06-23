// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CCSessionItemUI.generated.h"

class UTextBlock;

UCLASS()
class PARCHEESI_API UCCSessionItemUI : public UUserWidget
{
    GENERATED_BODY()

public:
    void SetSessionWidgetData(FText ServerName, int Players, int Ping);

protected:
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* ServerNameText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* PlayersText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* PingText;
};
