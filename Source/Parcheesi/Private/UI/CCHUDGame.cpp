// Copyright Cats and Cubes. All Rights Reserved.

#include "UI/CCHUDGame.h"
#include "UI/Widgets/CCGameLobbyUI.h"
#include "UI/Widgets/CCPlayersTurnContainer.h"
#include "UI/Widgets/CCGameTurnButtons.h"
#include "UI/Widgets/CCQuickMenuWidget.h"
#include "Framework/CCPlayerPawnGame.h"
#include "Framework/CCControllerGame.h"
#include "Kismet/GameplayStatics.h"
#include "Framework/CCGameModeBaseGame.h"
#include "GameFramework/PlayerController.h"
#include "CCCoreTypes.h"

void ACCHUDGame::BeginPlay()
{
    check(LobbyWidgetClass);
    check(TurnInfoContainerWidgetClass);
    check(GameTurnButtonsWidgetClass);
    check(QuickMenuWidgetClass);

    LobbyWidget = CreateWidget<UCCGameLobbyUI>(GetWorld(), LobbyWidgetClass);
    LobbyWidget->AddToViewport();
    LobbyWidget->OnStartGameButtonPressedEvent.AddDynamic(this, &ACCHUDGame::StartGameFromLobby);
    LobbyWidget->OnColorButtonPressed.AddDynamic(this, &ACCHUDGame::SelectColorInLobby);

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

    OwningPlayerPawn = Cast<ACCPlayerPawnGame>(GetOwningPawn());
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
    OwningPlayerPawn->Server_SpawnDice(true, true ,2);
}

void ACCHUDGame::MovePawn() {
    OwningPlayerPawn->Server_MoveSelectedPawn();
}

void ACCHUDGame::ShowTurnButtons()
{
    GameTurnButtonsWidget->SetVisibility(ESlateVisibility::Visible);
    GameTurnButtonsWidget->EnableRollButton();
}

void ACCHUDGame::HideTurnButtons()
{
    GameTurnButtonsWidget->SetVisibility(ESlateVisibility::Hidden);
}

void ACCHUDGame::SwitchQuickMenuVisibility()
{
    if (QuickMenuWidget->IsInViewport())
    {
        QuickMenuWidget->SwitchQuickMenuVisibility();
    }
}
