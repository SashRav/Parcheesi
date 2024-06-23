// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "CCCreateGameUI.generated.h"

class UCCMenuButtonUI;
class ACCHUDMainMenu;
class FOnlineSessionSettings;

UCLASS()
class PARCHEESI_API UCCCreateGameUI : public UUserWidget
{
    GENERATED_BODY()
protected:
    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    class UCCMenuButtonUI* CreateSessionButton;

    UPROPERTY(meta = (BindWidget))
    class UCCMenuButtonUI* BackToMenuButton;

    UFUNCTION()
    void CreateSessionButtonClicked();

    UFUNCTION()
    void BackToMenuButtonlicked();

    void CreateSession(int32 NumPublicConnections, bool IsLANMatch);

    virtual void OnCreateSessionComplete(FName ServerName, bool Succeeded);

private:
    UPROPERTY()
    ACCHUDMainMenu* MainMenuHUD;

    IOnlineSessionPtr SessionInterface;

    // TSharedPtr<FOnlineSessionSettings> LastSessionSettings;
};
