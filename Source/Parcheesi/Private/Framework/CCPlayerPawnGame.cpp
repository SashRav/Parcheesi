// Copyright Cats and Cubes. All Rights Reserved.

#include "Framework/CCPlayerPawnGame.h"
#include "Framework/CCGameModeBaseGame.h"
#include "Framework/CCGameStateGame.h"
#include "Framework/CCControllerGame.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerController.h"
#include "BoardItems/CCDice.h"
#include "BoardItems/CCPawn.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "Components/CCDiceComponent.h"
#include "Components/CCSelectItem.h"
#include "Components/CCPawnManagerComponent.h"
#include "Net/UnrealNetwork.h"

ACCPlayerPawnGame::ACCPlayerPawnGame()
{
    DiceComponent = CreateDefaultSubobject<UCCDiceComponent>(TEXT("DiceComponent"));
    SelectItemDiceComponent = CreateDefaultSubobject<UCCSelectItem>(TEXT("SelectionDiceComponent"));
    SelectItemPawnComponent = CreateDefaultSubobject<UCCSelectItem>(TEXT("SelectionPawnComponent"));
    PawnManagerComponent = CreateDefaultSubobject<UCCPawnManagerComponent>(TEXT("PawnManagerComponent"));

    PawnManagerComponent->OnPawnMovementFinished.AddDynamic(this, &ACCPlayerPawnGame::Multicast_HandlePawnMovementFinished);
    PawnManagerComponent->OnGameFinished.AddDynamic(this, &ACCPlayerPawnGame::Server_HandleGameFinished);
}

void ACCPlayerPawnGame::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ACCPlayerPawnGame, PlayerTagName);
}

void ACCPlayerPawnGame::BeginPlay()
{
    Super::BeginPlay();

    check(ClickOnBoardAction);

    ServerGameMode = Cast<ACCGameModeBaseGame>(UGameplayStatics::GetGameMode(GetWorld()));
    ServerGameState = Cast<ACCGameStateGame>(UGameplayStatics::GetGameState(GetWorld()));
    OwningPlayerController = Cast<ACCControllerGame>(GetController());

    DiceComponent->OnDiceRollingEnd.AddDynamic(this, &ACCPlayerPawnGame::Server_CheckIfCanEnableEndTurn);
    ServerGameState->OnSelectingColorInLobby.AddDynamic(this, &ACCPlayerPawnGame::Server_UpdateLobbySelection);
    ServerGameState->OnUpdatingSettingsInLobby.AddDynamic(this, &ACCPlayerPawnGame::Server_UpdateLobbySettings);
    ServerGameState->OnPlayersCountChanged.AddDynamic(this, &ACCPlayerPawnGame::Server_UpdateLobbyPlayers);

    Server_UpdateLobbySelection();
    Server_UpdateLobbySettings();
    Server_UpdateLobbyPlayers();
}

void ACCPlayerPawnGame::SetupPlayerInputComponent(UInputComponent* NewInputComponent)
{
    Super::SetupPlayerInputComponent(InputComponent);
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
    {
        EnhancedInputComponent->BindAction(ClickOnBoardAction, ETriggerEvent::Started, this, &ACCPlayerPawnGame::ClickOnBoard);
    }
}

void ACCPlayerPawnGame::Server_RollDices_Implementation()
{
    bDicesSpawned = true;
    DiceComponent->RollDices();
}

void ACCPlayerPawnGame::Server_UpdateSelectedColor_Implementation(const FName& ColorTag)
{
    UE_LOG(LogTemp, Display, TEXT("Color Tag clicked: %s"), *ColorTag.ToString());

    FPlayerInfo PlayerInfo;
    PlayerInfo.Tag = ColorTag;

    if (OwningPlayerController && OwningPlayerController->HasAuthority()) // Server don't have Ready button and it is ready by default
        PlayerInfo.bIsReady = true;
    else
        PlayerInfo.bIsReady = false;

    ServerGameMode->ChangePlayerInfo(GetPlayerState()->GetUniqueId(), PlayerInfo);
}

void ACCPlayerPawnGame::Server_PlayerIsReady_Implementation(bool bIsReady)
{
    TMap<FUniqueNetIdRepl, FPlayerInfo> PlayersData = ServerGameState->GetAllPlayersData();

    FUniqueNetIdRepl LocalID = GetPlayerState()->GetUniqueId();
    FPlayerInfo PlayerInfo = *PlayersData.Find(LocalID);
    PlayerInfo.bIsReady = bIsReady;

    ServerGameMode->ChangePlayerInfo(GetPlayerState()->GetUniqueId(), PlayerInfo);
}

void ACCPlayerPawnGame::Server_EndPlayerTurn_Implementation()
{
    UE_LOG(LogTemp, Display, TEXT("End turn from Pawn"));
    Server_CleanSelectionData();
    Multicast_SetCurrentTurn(false);
    Server_CleanAllDices();
    ServerGameMode->StartNextTurn();
    bDicesSpawned = false;
}

void ACCPlayerPawnGame::Server_DebugEndPlayerTurn_Implementation()
{
    UE_LOG(LogTemp, Display, TEXT("Debug End turn from Pawn"));
    Server_EndPlayerTurn();
}

void ACCPlayerPawnGame::Server_CleanAllDices_Implementation()
{
    SelectedDiceActor = nullptr;
    for (ACCDice* Dice : ServerGameState->GetSpawnedDices())
    {
        ServerGameState->RemoveDice(Dice);
        Dice->Destroy();
    }
    // Additional cleaning up for array
    ServerGameState->CleanSpawnedDicesArray();
}

void ACCPlayerPawnGame::Server_CleanSelectionData_Implementation()
{
    if (SelectedDiceActor)
    {
        Client_VisualDeselectActor(SelectItemDiceComponent, SelectedDiceActor->GetStaticMeshComponent());
        SelectedDiceActor = nullptr;
    }
    if (SelectedPawnActor)
    {
        Client_VisualDeselectActor(SelectItemPawnComponent, SelectedPawnActor->PawnMeshComponent);
        SelectedPawnActor = nullptr;
    }
}

void ACCPlayerPawnGame::Multicast_SetCurrentTurn_Implementation(bool Turn)
{
    bCurrentTurn = Turn;
}

void ACCPlayerPawnGame::Client_EnableTurnButton_Implementation()
{
    OwningPlayerController->Client_EnableEndTurnButton();
}

void ACCPlayerPawnGame::UpdateSelectedDiceOnUI()
{
    // Needed to get Player controller because it does not exist in the context
    ACCControllerGame* PlayerController = Cast<ACCControllerGame>(GetController());
    if (!PlayerController)
    {
        UE_LOG(LogTemp, Log, TEXT("OwningPlayerController is not valid in UpdateSelectedDiceOnUI"));
        return;
    }
    if (!SelectedDiceActor)
    {
        UE_LOG(LogTemp, Log, TEXT("SelectedDiceActor is not valid"));
        return;
    }
    PlayerController->Client_SetDiceSideOnUI(SelectedDiceActor->GetDiceSide());
}

void ACCPlayerPawnGame::Server_TrySwitchMovePawnButtonIsEnabled_Implementation(bool State)
{
    // Needed to get Player controller because it does not exist in the context
    ACCControllerGame* PlayerController = Cast<ACCControllerGame>(GetController());
    if (!PlayerController)
    {
        UE_LOG(LogTemp, Log, TEXT("OwningPlayerController is not valid in SwitchMovePawnButtonIsEnabled"));
        return;
    }

    PlayerController->Client_SwitchMovePawnButtonIsEnabled(false);

    if (SelectedDiceActor && SelectedPawnActor)
    {
        if (PawnManagerComponent->CheckPawnCanMove(SelectedPawnActor, SelectedDiceActor->GetDiceSide()))
            PlayerController->Client_SwitchMovePawnButtonIsEnabled(true);
    }
}

void ACCPlayerPawnGame::Client_VisualDeselectActor_Implementation(UCCSelectItem* Component, UMeshComponent* Mesh)
{
    Component->DeselectThisItem(Mesh);
}

void ACCPlayerPawnGame::Client_VisualSelectActor_Implementation(UCCSelectItem* Component, UMeshComponent* Mesh)
{
    Component->SelectThisItem(Mesh);
}

void ACCPlayerPawnGame::Server_SelectDiceActor_Implementation(ACCDice* HitDice)
{
    if (SelectedDiceActor)
        Client_VisualDeselectActor(SelectItemDiceComponent, SelectedDiceActor->GetStaticMeshComponent());

    SelectedDiceActor = HitDice;
    Client_VisualSelectActor(SelectItemDiceComponent, SelectedDiceActor->GetStaticMeshComponent());

    UpdateSelectedDiceOnUI();
}

void ACCPlayerPawnGame::Server_SelectPawnActor_Implementation(ACCPawn* HitPawn)
{
    if (HitPawn->Tags[0] != PlayerTagName)
        return;

    if (SelectedPawnActor)
        Client_VisualDeselectActor(SelectItemPawnComponent, SelectedPawnActor->PawnMeshComponent);

    SelectedPawnActor = HitPawn;
    Client_VisualSelectActor(SelectItemPawnComponent, SelectedPawnActor->PawnMeshComponent);

    UE_LOG(LogTemp, Log, TEXT("Pawn Actor Selected"));
}

void ACCPlayerPawnGame::ClickOnBoard()
{
    if (!bCurrentTurn || bIsPawnMoving)
        return;

    if (OwningPlayerController)
    {
        FHitResult HitResult;
        if (OwningPlayerController->GetHitResultUnderCursor(ECC_Visibility, false, HitResult))
        {
            if (ACCDice* HitDice = Cast<ACCDice>(HitResult.GetActor()))
            {
                Server_SelectDiceActor(HitDice);
            }
            else if (ACCPawn* HitPawn = Cast<ACCPawn>(HitResult.GetActor()))
            {
                Server_SelectPawnActor(HitPawn);
            }
        }
        if (bDicesSpawned)
            Server_CheckIfCanEnableEndTurn();

        Server_TrySwitchMovePawnButtonIsEnabled(true);
    }
}

void ACCPlayerPawnGame::Server_MoveSelectedPawn_Implementation()
{
    if (!SelectedDiceActor || !SelectedPawnActor || bIsPawnMoving)
        return;

    PawnManagerComponent->MoveSelectedPawn(SelectedPawnActor, SelectedDiceActor->GetDiceSide());
    Multicast_HandlePawnMovementStarted();

    Client_VisualDeselectActor(SelectItemPawnComponent, SelectedPawnActor->PawnMeshComponent);

    ServerGameState->RemoveDice(SelectedDiceActor); // Exists for debug purpose
    if (SelectedDiceActor->bDestryWhenUsed)
    {
        SelectedDiceActor->Destroy();
    }

    ACCControllerGame* PlayerController = Cast<ACCControllerGame>(GetController());
    PlayerController->Client_SetDiceSideOnUI(0);

    SelectedPawnActor = nullptr;
    SelectedDiceActor = nullptr;
}

void ACCPlayerPawnGame::Multicast_HandlePawnMovementStarted_Implementation()
{
    bIsPawnMoving = true;
}

void ACCPlayerPawnGame::Multicast_HandlePawnMovementFinished_Implementation()
{
    bIsPawnMoving = false;
    Server_CheckIfCanEnableEndTurn();
}

void ACCPlayerPawnGame::Server_CheckIsAnyMoveAvailable_Implementation()
{
    TArray<ACCPawn*> AllPawns = ServerGameState->GetAllPawns();
    TArray<ACCDice*> SpawnedDices = ServerGameState->GetSpawnedDices();
    bIsAnyPawnCanMove = false;

    if (SpawnedDices.Num() == 0)
        return;

    TArray<ACCPawn*> PlayerPawns;

    for (ACCPawn* Pawn : AllPawns)
    {
        if (Pawn->Tags[0] == PlayerTagName)
            PlayerPawns.Add(Pawn);
    }
    for (ACCPawn* MyPawn : PlayerPawns)
    {
        for (ACCDice* Dice : SpawnedDices)
        {
            if (PawnManagerComponent->CheckPawnCanMove(MyPawn, Dice->GetDiceSide()))
            {
                bIsAnyPawnCanMove = true;
                return;
            }
        }
    }
}

void ACCPlayerPawnGame::Server_CheckIfCanEnableEndTurn_Implementation()
{
    Server_CheckIsAnyMoveAvailable();
    if (!bIsAnyPawnCanMove)
        Client_EnableTurnButton();
}

void ACCPlayerPawnGame::Server_HandleGameFinished_Implementation()
{
    ServerGameMode->FinishGame(PlayerTagName);
}

void ACCPlayerPawnGame::Server_UpdateLobbySelection_Implementation()
{
    TArray<FAllPlayersData> AllPlayersData;
    TMap<FUniqueNetIdRepl, FPlayerInfo> AllPlayersDataMap = ServerGameState->GetAllPlayersData();

    if (AllPlayersDataMap.Num() > 0)
        for (TPair<FUniqueNetIdRepl, FPlayerInfo>& Player : AllPlayersDataMap)
        {
            FAllPlayersData NewPlayerData;
            FPlayerInfo DataValue = Player.Value;
            NewPlayerData.PlayerName = FText::FromString(Player.Key.GetUniqueNetId()->ToString().Right(5));
            NewPlayerData.Tag = DataValue.Tag;
            NewPlayerData.bIsReady = DataValue.bIsReady;

            AllPlayersData.Add(NewPlayerData);
        }

    Client_UpdateLobbySelection(AllPlayersData);
}

void ACCPlayerPawnGame::Client_UpdateLobbySelection_Implementation(const TArray<FAllPlayersData>& AllPlayersData)
{
    if (OwningPlayerController)
        OwningPlayerController->Client_UpdateLobbySelection(AllPlayersData);
}

void ACCPlayerPawnGame::Server_UpdateLobbySettings_Implementation()
{
    FGameSettings GameSettings = ServerGameState->GetGameSettings();
    Client_UpdateLobbySettings(GameSettings);
}

void ACCPlayerPawnGame::Client_UpdateLobbySettings_Implementation(FGameSettings GameSettings)
{
    if (OwningPlayerController)
        OwningPlayerController->Client_UpdateLobbySettings(GameSettings);
}

void ACCPlayerPawnGame::Server_UpdateLobbyPlayers_Implementation()
{
    TArray<FUniqueNetIdRepl> PlayersArray;
    ServerGameState->GetAllPlayersData().GetKeys(PlayersArray);

    Client_UpdateLobbyPlayers(PlayersArray);
}

void ACCPlayerPawnGame::Client_UpdateLobbyPlayers_Implementation(const TArray<FUniqueNetIdRepl>& AllPlayers)
{
    if (OwningPlayerController)
        OwningPlayerController->Client_UpdatePlayersList(AllPlayers);
}

void ACCPlayerPawnGame::Server_DisconnectPlayer_Implementation(FUniqueNetIdRepl PlayerID) 
{
    ServerGameMode->DisconnectPlayer(PlayerID);
}