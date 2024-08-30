// Copyright Cats and Cubes. All Rights Reserved.

#include "Framework/CCControllerGame.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "UI/CCHUDGame.h"
#include "CCCoreTypes.h"

void ACCControllerGame::BeginPlay()
{
    Super::BeginPlay();

    check(GameMappingContext);
    check(LevelInputContext);
    check(QuickMenuAction);

    FInputModeGameAndUI InputModeData;
    InputModeData.SetHideCursorDuringCapture(false);
    SetInputMode(InputModeData);
    
    bShowMouseCursor = true;

    OwningHUD = Cast<ACCHUDGame>(GetHUD());

    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        Subsystem->AddMappingContext(LevelInputContext, 0);
        Subsystem->AddMappingContext(GameMappingContext, 1);
    }
}

void ACCControllerGame::SetupInputComponent()
{
    Super::SetupInputComponent();
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
    {
        EnhancedInputComponent->BindAction(
            QuickMenuAction, ETriggerEvent::Started, this, &ACCControllerGame::Client_SwitchQuickMenuVisibility);
    }
}

void ACCControllerGame::Client_SwitchQuickMenuVisibility_Implementation() {
    if (OwningHUD)
        OwningHUD->SwitchQuickMenuVisibility();
}

void ACCControllerGame::Client_StartGameFromController_Implementation()
{
    if (OwningHUD)
        OwningHUD->RemoveLobbyWidget();
}

void ACCControllerGame::Client_UpdateTurnWidgets_Implementation(const TArray<FPlayersTurnData>& PlayersTurnData)
{
    if (OwningHUD)
        OwningHUD->UpdateTurnWidgets(PlayersTurnData);
}

void ACCControllerGame::Client_ShowTurnButtonsWidget_Implementation()
{
    if (OwningHUD)
        OwningHUD->ShowTurnButtons();
}

void ACCControllerGame::Client_HideTurnButtonsWidget_Implementation()
{
    if (OwningHUD)
        OwningHUD->HideTurnButtons();
}