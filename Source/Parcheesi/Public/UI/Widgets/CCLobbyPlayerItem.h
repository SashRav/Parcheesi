// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CCLobbyPlayerItem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKickPlayerButtonPressed, FUniqueNetIdRepl, PlayerID);

class UTextBlock;
class UButton;

UCLASS()
class PARCHEESI_API UCCLobbyPlayerItem : public UUserWidget
{
    GENERATED_BODY()

public:
    void SetPlayerData(FText Name, FUniqueNetIdRepl PlayerID, bool bEnableButton);

    UPROPERTY(BlueprintAssignable)
    FOnKickPlayerButtonPressed OnKickPlayerButtonPressed;

protected:
    virtual void NativeConstruct() override;
    
    UFUNCTION()
    void KickPlayer();

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* T_PlayerName;

    // Global lobby buttons
    UPROPERTY(meta = (BindWidget))
    class UButton* B_KickPlayer;

    UPROPERTY(meta = (BindWidget))
    FUniqueNetIdRepl PlayerNetID;
};
