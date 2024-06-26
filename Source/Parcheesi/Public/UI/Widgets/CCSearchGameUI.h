// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "CCSearchGameUI.generated.h"

class UCCMenuButtonUI;
class ACCHUDMainMenu;
class UCCSessionItemUI;
class UScrollBox;
class UTextBlock;

UCLASS()
class PARCHEESI_API UCCSearchGameUI : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;
    virtual void OnFindSessionsComplete(bool Succeeded);
    virtual void SetSelectedSession(FOnlineSessionSearchResult Result);
    virtual void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UCCSessionItemUI> SessionResultWidgetClass;

    UPROPERTY(meta = (BindWidget))
    class UScrollBox* ServerScrollBox;

    UPROPERTY(meta = (BindWidget))
    class UCCMenuButtonUI* RefreshButton;

    UPROPERTY(meta = (BindWidget))
    class UCCMenuButtonUI* BackToMenuButton;

    UPROPERTY(meta = (BindWidget))
    class UCCMenuButtonUI* JoinGameButton;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* StatusTextBox;

    UFUNCTION()
    void RefreshButtonClicked();

    UFUNCTION()
    void JoinGameButtonClicked();

    UFUNCTION()
    void BackToMenuButtonlicked();

private:
    UPROPERTY()
    ACCHUDMainMenu* MainMenuHUD;

    UPROPERTY()
    UCCSessionItemUI* SessionWidget = nullptr;

    IOnlineSessionPtr SessionInterface;

    TSharedPtr<FOnlineSessionSearch> SessionSearch;
    
    FOnlineSessionSearchResult SelectedSessionResult;


    void AddServerLine(FOnlineSessionSearchResult Result);
};
