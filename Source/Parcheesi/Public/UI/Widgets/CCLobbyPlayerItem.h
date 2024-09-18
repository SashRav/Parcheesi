// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CCLobbyPlayerItem.generated.h"

class UTextBlock;

UCLASS()
class PARCHEESI_API UCCLobbyPlayerItem : public UUserWidget
{
    GENERATED_BODY()

public:
    void SetPlayerName(FText Name);

protected:
    
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* T_PlayerName;
};
