// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessionSettings.h"
#include "CCSessionItemUI.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSessionItemPressed, FOnlineSessionSearchResult);

class UTextBlock;
class UBorder;

UCLASS()
class PARCHEESI_API UCCSessionItemUI : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
   
    FOnSessionItemPressed OnSessionItemPressedEvent;

    void SetSessionWidgetData(FName ServerName, int Players, int MaxPlayers, int Ping);
    void SetSessionData(FOnlineSessionSearchResult SessionData);
    void CleanSelection();

protected:
    FOnlineSessionSearchResult LocalSessionData;

    UPROPERTY(meta = (BindWidget))
    class UBorder* SelectingBorder;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* ServerNameText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* PlayersText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* PingText;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UCCSessionItemUI> SessionWidget;


};
