// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CCHUDMainMenu.generated.h"

UCLASS()
class PARCHEESI_API ACCHUDMainMenu : public AHUD
{
    GENERATED_BODY()

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> MainMenuWidgetClass;

    virtual void BeginPlay() override;

private:
    UPROPERTY()
    UUserWidget* MainMenuWidget = nullptr;
};
