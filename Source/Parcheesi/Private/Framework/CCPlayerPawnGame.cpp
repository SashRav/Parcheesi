// Copyright Cats and Cubes. All Rights Reserved.

#include "Framework/CCPlayerPawnGame.h"
#include "Framework/CCGameModeBaseGame.h"
#include "Framework/CCControllerGame.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerState.h"
// #include "Online/CoreOnlinePackage.h"

ACCPlayerPawnGame::ACCPlayerPawnGame() {}

void ACCPlayerPawnGame::BeginPlay()
{
    Super::BeginPlay();
}
    // Client_GetPlayerId_Implementation();
    //Server_AddPlayerToList_Implementation();
   // const FName PlayerDefaultTag = "Lobby";
    // const FUniqueNetIdRepl PlayerNetId = GetPlayerState()->GetUniqueId();
    // FString UserIdSerialized = PlayerNetId.GetUniqueNetId()->ToString();
    // APlayerState* State = UGameplayStatics::GetPlayerStateFromUniqueNetId(GetWorld(), PlayerNetId);
    //  if (State)
    //      UE_LOG(LogTemp, Display, TEXT("Player state found"));
    //if (GetPlayerState())
    //{

    //    FUniqueNetIdRepl PlayerNetId = GetPlayerState()->GetUniqueId();
    //    if (PlayerNetId.IsValid())
    //        if (GetWorld())
    //        {
    //            ACCGameModeBaseGame* GameMode = Cast<ACCGameModeBaseGame>(UGameplayStatics::GetGameMode(GetWorld()));
    //            if (GameMode)
    //                GameMode->AddPlayerToAllPlayersData(PlayerNetId, PlayerDefaultTag);
    //        }
    //}

//
//void ACCPlayerPawnGame::Client_GetPlayerId_Implementation()
//{
//    //PlayerNetId /* = GetPlayerState()->GetUniqueId();
//     /*if (PlayerNetId.IsValid())
//         FString UserIdSerialized = PlayerNetId.GetUniqueNetId()->ToString();*/
//}
//
//void ACCPlayerPawnGame::Server_AddPlayerToList_Implementation()
//{
//    const FName PlayerDefaultTag = "Lobby";
//    // const FUniqueNetIdRepl PlayerNetId = GetPlayerState()->GetUniqueId();
//    // FString UserIdSerialized = PlayerNetId.GetUniqueNetId()->ToString();
//    // APlayerState* State = UGameplayStatics::GetPlayerStateFromUniqueNetId(GetWorld(), PlayerNetId);
//    //  if (State)
//    //      UE_LOG(LogTemp, Display, TEXT("Player state found"));
//    if (GetPlayerState())
//    {
//
//        FUniqueNetIdRepl PlayerNetId = GetPlayerState()->GetUniqueId();
//        if (PlayerNetId.IsValid())
//            if (GetWorld())
//            {
//                ACCGameModeBaseGame* GameMode = Cast<ACCGameModeBaseGame>(UGameplayStatics::GetGameMode(GetWorld()));
//                if (GameMode)
//                    GameMode->AddPlayerToAllPlayersData(PlayerNetId, PlayerDefaultTag);
//            }
//    }
//}
