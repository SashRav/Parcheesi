// Copyright Cats and Cubes. All Rights Reserved.

#include "Framework/CCGameModeBaseGame.h"
#include "Framework/CCGameStateGame.h"
#include "Framework/CCPlayerPawnGame.h"
#include "Framework/CCPlayerStateGame.h"
#include "Framework/CCControllerGame.h"
#include "Framework/CCSpectatorPawnGame.h"
#include "Framework/CCControllerLobby.h"
#include "UI/CCHUDLobby.h"
#include "UI/CCHUDGame.h"

ACCGameModeBaseGame::ACCGameModeBaseGame()
{
    PlayerControllerClass = ACCControllerLobby::StaticClass();
    HUDClass = ACCHUDLobby::StaticClass();
    GameStateClass = ACCGameStateGame::StaticClass();
}
void ACCGameModeBaseGame::StartNewGame()
{
    check(GameHUDClass);
    check(PlayerControllerGameClass);
    check(PlayerStateGameClass);
    check(SpectatorPawnGameClass);
    check(PlayerPawnGameClass);

    HUDClass = GameHUDClass;
    PlayerControllerClass = PlayerControllerGameClass;
    PlayerStateClass = PlayerStateGameClass;
    SpectatorClass = SpectatorPawnGameClass;
    DefaultPawnClass = PlayerPawnGameClass;
}