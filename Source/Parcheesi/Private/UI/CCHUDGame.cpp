// Copyright Cats and Cubes. All Rights Reserved.

#include "UI/CCHUDGame.h"
#include "UI/Widgets/CCGameLobbyUI.h"
#include "UI/Widgets/CCPlayersTurnContainer.h"
#include "UI/Widgets/CCGameTurnButtons.h"
#include "UI/Widgets/CCQuickMenuWidget.h"
#include "UI/Widgets/CCWinWidget.h"
#include "Framework/CCPlayerPawnGame.h"
#include "Kismet/GameplayStatics.h"
#include "Framework/CCGameModeBaseGame.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "CCCoreTypes.h"

void ACCHUDGame::BeginPlay()
{
    Super::BeginPlay();

    check(LobbyWidgetClass);
    check(TurnInfoContainerWidgetClass);
    check(GameTurnButtonsWidgetClass);
    check(QuickMenuWidgetClass);
    check(WinWidgetClass);

    TurnInfoContainerWidget = CreateWidget<UCCPlayersTurnContainer>(GetWorld(), TurnInfoContainerWidgetClass);
    TurnInfoContainerWidget->AddToViewport();

    GameTurnButtonsWidget = CreateWidget<UCCGameTurnButtons>(GetWorld(), GameTurnButtonsWidgetClass);
    GameTurnButtonsWidget->SetVisibility(ESlateVisibility::Hidden);
    GameTurnButtonsWidget->AddToViewport();
    GameTurnButtonsWidget->OnEndTurnPressedEvent.AddDynamic(this, &ACCHUDGame::EndPlayerTurn);
    GameTurnButtonsWidget->OnDebugEndTurnPressedEvent.AddDynamic(this, &ACCHUDGame::DebugEndPlayerTurn);
    GameTurnButtonsWidget->OnRollDicePressedEvent.AddDynamic(this, &ACCHUDGame::RollDice);
    GameTurnButtonsWidget->OnMovePawnPressedEvent.AddDynamic(this, &ACCHUDGame::MovePawn);

    QuickMenuWidget = CreateWidget<UCCQuickMenuWidget>(GetWorld(), QuickMenuWidgetClass);
    QuickMenuWidget->OnExitToMenuButtonPressedEvent.AddDynamic(this, &ACCHUDGame::DisconnectCurrentPlayerFromGame);

    WinWidget = CreateWidget<UCCWinWidget>(GetWorld(), WinWidgetClass);
    WinWidget->SetVisibility(ESlateVisibility::Hidden);
    WinWidget->AddToViewport();

    OwningPlayerPawn = Cast<ACCPlayerPawnGame>(GetOwningPawn());

    LobbyWidget = CreateWidget<UCCGameLobbyUI>(GetWorld(), LobbyWidgetClass);
    LobbyWidget->AddToViewport();
    LobbyWidget->SetVisibility(ESlateVisibility::Hidden);
    LobbyWidget->OnStartGameButtonPressedEvent.AddDynamic(this, &ACCHUDGame::StartGameFromLobby);
    LobbyWidget->OnColorButtonPressed.AddDynamic(this, &ACCHUDGame::SelectColorInLobby);
    LobbyWidget->OnReadyButtonPressed.AddDynamic(this, &ACCHUDGame::PlayerReady);
    LobbyWidget->OnSaveSettingsButtonPressed.AddDynamic(this, &ACCHUDGame::SaveGameSettings);
    LobbyWidget->OnExitToMenuButtonPressed.AddDynamic(this, &ACCHUDGame::DisconnectCurrentPlayerFromLobby);
    LobbyWidget->OnKickPlayerFromLobby.AddDynamic(this, &ACCHUDGame::RemovePlayerFromGame);
}

void ACCHUDGame::CreateLobbyWidget()
{
    LobbyWidget->SetVisibility(ESlateVisibility::Visible);
}

void ACCHUDGame::AddQuickMenuWidget()
{
    QuickMenuWidget->AddToViewport();
}

void ACCHUDGame::RemoveLobbyWidget()
{
    if (LobbyWidget->IsInViewport())
    {
        LobbyWidget->RemoveFromParent();
        UE_LOG(LogTemp, Display, TEXT("Lobby widget removed"));
    }
    else
        UE_LOG(LogTemp, Display, TEXT("Lobby widget already removed"));
    AddQuickMenuWidget();
}

void ACCHUDGame::UpdateTurnWidgets(const TArray<FPlayersTurnData>& PlayersTurnData)
{
    TurnInfoContainerWidget->UpdatePlayersTurnData(PlayersTurnData);
}

void ACCHUDGame::StartGameFromLobby()
{
    ACCGameModeBaseGame* ServerGameMode = Cast<ACCGameModeBaseGame>(UGameplayStatics::GetGameMode(GetWorld()));
    ServerGameMode->StartNewGame();
}

void ACCHUDGame::SelectColorInLobby(FName ColorTag)
{
    OwningPlayerPawn->Server_UpdateSelectedColor(ColorTag);
}

void ACCHUDGame::PlayerReady(bool bIsReady)
{
    OwningPlayerPawn->Server_PlayerIsReady(bIsReady);
}

void ACCHUDGame::SaveGameSettings(FGameSettings Settings)
{
    ACCGameModeBaseGame* ServerGameMode = Cast<ACCGameModeBaseGame>(UGameplayStatics::GetGameMode(GetWorld()));
    ServerGameMode->SaveGameSettings(Settings);
}

void ACCHUDGame::EndPlayerTurn()
{
    OwningPlayerPawn->Server_EndPlayerTurn();
}

void ACCHUDGame::DebugEndPlayerTurn()
{
    OwningPlayerPawn->Server_DebugEndPlayerTurn();
}

void ACCHUDGame::RollDice()
{
    OwningPlayerPawn->RollDice();
}

void ACCHUDGame::MovePawn()
{
    OwningPlayerPawn->Server_MoveSelectedPawn();
}

void ACCHUDGame::ShowTurnButtons()
{
    GameTurnButtonsWidget->SetVisibility(ESlateVisibility::Visible);
    GameTurnButtonsWidget->EnableRollButton();
    GameTurnButtonsWidget->SetIsEnabledEndTurnButton(false);
    GameTurnButtonsWidget->SetIsEnabledMovePawnButton(false);
}

void ACCHUDGame::HideTurnButtons()
{
    GameTurnButtonsWidget->SetVisibility(ESlateVisibility::Hidden);
}

void ACCHUDGame::EnableEndTurnButton()
{
    GameTurnButtonsWidget->SetIsEnabledEndTurnButton(true);
}

void ACCHUDGame::SetSeclectedDiceSideOnUI(int32 Side)
{
    GameTurnButtonsWidget->SetDiceSideOnUI(Side);
}

void ACCHUDGame::SwitchQuickMenuVisibility()
{
    if (QuickMenuWidget->IsInViewport())
    {
        QuickMenuWidget->SwitchQuickMenuVisibility();
    }
}

void ACCHUDGame::SwitchMovePawnButtonIsEnabled(bool State)
{
    GameTurnButtonsWidget->SetIsEnabledMovePawnButton(State);
}

void ACCHUDGame::ShowWinWidget(FText WinnerName)
{
    WinWidget->SetWinner(WinnerName);
    WinWidget->SetVisibility(ESlateVisibility::Visible);
}

void ACCHUDGame::UpdateLobbySelection(const TArray<FAllPlayersData>& AllPlayersData)
{
    LobbyWidget->UpdateSelectionStatus(AllPlayersData);
}

void ACCHUDGame::UpdateLobbySettings(FGameSettings GameSettings)
{
    LobbyWidget->UpdateSettings(GameSettings);
}

void ACCHUDGame::UpdatePlayersList(const TArray<FUniqueNetIdRepl>& AllPlayers)
{
    LobbyWidget->UpdatePlayersList(AllPlayers);
}

void ACCHUDGame::RemovePlayerFromGame(FUniqueNetIdRepl PlayerID)
{
    OwningPlayerPawn->Server_DisconnectPlayer(PlayerID);
}

void ACCHUDGame::DisconnectCurrentPlayerFromLobby()
{
    // Add checks is player ready or not
    RemovePlayerFromGame(OwningPlayerPawn->GetPlayerState()->GetUniqueId());
}

void ACCHUDGame::DisconnectCurrentPlayerFromGame()
{
    // Will be additional functional to set bot instead of player
    RemovePlayerFromGame(OwningPlayerPawn->GetPlayerState()->GetUniqueId());
}