// Copyright Cats and Cubes. All Rights Reserved.

#include "Framework/CCControllerGame.h"
#include "Framework/CCPlayerPawnGame.h"
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

void ACCControllerGame::Client_SwitchQuickMenuVisibility_Implementation()
{
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

    ACCPlayerPawnGame* PlayerPawn = Cast<ACCPlayerPawnGame>(GetPawn());
    if (PlayerPawn)
        PlayerPawn->Multicast_SetCurrentTurn(true);
}

void ACCControllerGame::Client_HideTurnButtonsWidget_Implementation()
{
    if (OwningHUD)
        OwningHUD->HideTurnButtons();
}

void ACCControllerGame::Client_EnableEndTurnButton_Implementation()
{
    if (OwningHUD)
        OwningHUD->EnableEndTurnButton();
}

void ACCControllerGame::Client_SetDiceSideOnUI_Implementation(int32 Side)
{
    if (OwningHUD)
        OwningHUD->SetSeclectedDiceSideOnUI(Side);
}

void ACCControllerGame::Client_SwitchMovePawnButtonIsEnabled_Implementation(bool State)
{
    if (OwningHUD)
        OwningHUD->SwitchMovePawnButtonIsEnabled(State);
}

void ACCControllerGame::Client_ShowWinWidget_Implementation(const FText& WinnerName)
{
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        Subsystem->RemoveMappingContext(LevelInputContext);
        Subsystem->RemoveMappingContext(GameMappingContext);
    }
    bShowMouseCursor = true;
    if (OwningHUD)
        OwningHUD->ShowWinWidget(WinnerName);
}

void ACCControllerGame::Client_UpdateLobbySelection_Implementation(const TArray<FAllPlayersData>& AllPlayersData)
{
    if (OwningHUD)
        OwningHUD->UpdateLobbySelection(AllPlayersData);
}

void ACCControllerGame::Client_UpdateLobbySettings_Implementation(FGameSettings GameSettings)
{
    if (OwningHUD)
        OwningHUD->UpdateLobbySettings(GameSettings);
}

void ACCControllerGame::Client_UpdatePlayersList_Implementation(const TArray<FUniqueNetIdRepl>& AllPlayers)
{
    if (OwningHUD)
        OwningHUD->UpdatePlayersList(AllPlayers);
}
