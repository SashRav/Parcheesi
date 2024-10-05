// Copyright Cats and Cubes. All Rights Reserved.

#include "Framework/CCPlayerPawnGame.h"
#include "Framework/CCGameModeBaseGame.h"
#include "Framework/CCGameStateGame.h"
#include "Framework/CCControllerGame.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "BoardItems/CCDice.h"
#include "BoardItems/CCPawn.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "Components/CCDiceComponent.h"
#include "Components/CCSelectItem.h"
#include "Components/CCPawnManagerComponent.h"
#include "Components/TimelineComponent.h"
#include "Net/UnrealNetwork.h"
#include "Camera/CameraComponent.h"

ACCPlayerPawnGame::ACCPlayerPawnGame()
{
    DiceComponent = CreateDefaultSubobject<UCCDiceComponent>(TEXT("DiceComponent"));
    SelectItemDiceComponent = CreateDefaultSubobject<UCCSelectItem>(TEXT("SelectionDiceComponent"));
    SelectItemPawnComponent = CreateDefaultSubobject<UCCSelectItem>(TEXT("SelectionPawnComponent"));

    PawnManagerComponent = CreateDefaultSubobject<UCCPawnManagerComponent>(TEXT("PawnManagerComponent"));
    PawnManagerComponent->OnPawnMovementFinished.AddDynamic(this, &ACCPlayerPawnGame::Multicast_HandlePawnMovementFinished);
    PawnManagerComponent->OnGameFinished.AddDynamic(this, &ACCPlayerPawnGame::Server_HandleGameFinished);

    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
    SpringArmComponent->SetupAttachment(RootComponent);
    SpringArmComponent->bDoCollisionTest = false;

    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
    CameraComponent->SetupAttachment(SpringArmComponent);

    // Setup camera movement
    TimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("TimelineComponent"));

    PrimaryActorTick.bCanEverTick = false;
}

void ACCPlayerPawnGame::SetPlayerTagName(FName TagName)
{
    PlayerTagName = TagName;
    Client_SetCameraInitPosition(TagName);
}

void ACCPlayerPawnGame::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ACCPlayerPawnGame, PlayerTagName);
    DOREPLIFETIME(ACCPlayerPawnGame, SelectedPawnActor);
}

void ACCPlayerPawnGame::BeginPlay()
{
    Super::BeginPlay();

    check(ClickOnBoardAction);
    check(ZoomCameraAction);
    check(RotateCameraAction);
    check(DoubleClickOnBoardAction);
    check(ResetCameraAction);
    check(CameraMovementToDefaultCurve);
    check(CameraMovementToPawnCurve);

    ServerGameMode = Cast<ACCGameModeBaseGame>(UGameplayStatics::GetGameMode(GetWorld()));
    ServerGameState = Cast<ACCGameStateGame>(UGameplayStatics::GetGameState(GetWorld()));
    OwningPlayerController = Cast<ACCControllerGame>(GetController());

    SpringArmComponent->SetRelativeRotation(DefaultSpringArmRotation);
    SpringArmComponent->TargetArmLength = DefaultSpringArmLenght;
    SpringArmComponent->SocketOffset = FVector(0.0f, 0.0f, -700.0f);

    DefaultActorLocation = GetActorLocation();

    DiceComponent->OnDiceRollingEnd.AddDynamic(this, &ACCPlayerPawnGame::Server_CheckIfCanEnableEndTurn);

    FTimerHandle LobbyInitTimer;
    GetWorld()->GetTimerManager().SetTimer(LobbyInitTimer, this, &ACCPlayerPawnGame::InitLobby, 0.1f, false);
}

void ACCPlayerPawnGame::InitLobby()
{
    if (!ServerGameState)
        return;

    if (!ServerGameState->IsGameStarted())
    {
        if (OwningPlayerController)
            OwningPlayerController->Client_CreateLobbyWidget();

        ServerGameState->OnSelectingColorInLobby.AddDynamic(this, &ACCPlayerPawnGame::Server_UpdateLobbySelection);
        ServerGameState->OnUpdatingSettingsInLobby.AddDynamic(this, &ACCPlayerPawnGame::Server_UpdateLobbySettings);
        ServerGameState->OnPlayersCountChanged.AddDynamic(this, &ACCPlayerPawnGame::Server_UpdateLobbyPlayers);

        Server_UpdateLobbySelection();
        Server_UpdateLobbySettings();
        Server_UpdateLobbyPlayers();
    }
}

void ACCPlayerPawnGame::SetupPlayerInputComponent(UInputComponent* NewInputComponent)
{
    Super::SetupPlayerInputComponent(InputComponent);
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
    {
        EnhancedInputComponent->BindAction(ClickOnBoardAction, ETriggerEvent::Started, this, &ACCPlayerPawnGame::ClickOnBoard);
        EnhancedInputComponent->BindAction(ResetCameraAction, ETriggerEvent::Started, this, &ACCPlayerPawnGame::ResetCameraByClick);
        EnhancedInputComponent->BindAction(ZoomCameraAction, ETriggerEvent::Triggered, this, &ACCPlayerPawnGame::ZoomCamera);
        EnhancedInputComponent->BindAction(RotateCameraAction, ETriggerEvent::Triggered, this, &ACCPlayerPawnGame::RotateCamera);
        EnhancedInputComponent->BindAction(
            DoubleClickOnBoardAction, ETriggerEvent::Completed, this, &ACCPlayerPawnGame::DoubleClickOnBoard);
    }
}

void ACCPlayerPawnGame::Server_RollDices_Implementation()
{
    bDicesSpawned = true;
    DiceComponent->RollDices();
}

void ACCPlayerPawnGame::Server_UpdateSelectedColor_Implementation(const FName& ColorTag)
{
    if (!ServerGameMode)
        return;

    FPlayerInfo PlayerInfo;
    PlayerInfo.Tag = ColorTag;
    PlayerInfo.bIsReady = false;

    if (OwningPlayerController && OwningPlayerController->HasAuthority()) // Server don't have Ready button and it is ready by default
        PlayerInfo.bIsReady = true;

    ServerGameMode->ChangePlayerInfo(GetPlayerState()->GetUniqueId(), PlayerInfo);
}

void ACCPlayerPawnGame::Server_PlayerIsReady_Implementation(bool bIsReady)
{
    if (!ServerGameState)
        return;

    TMap<FUniqueNetIdRepl, FPlayerInfo> PlayersData = ServerGameState->GetAllPlayersData();

    FUniqueNetIdRepl LocalID = GetPlayerState()->GetUniqueId();
    FPlayerInfo PlayerInfo = *PlayersData.Find(LocalID);
    PlayerInfo.bIsReady = bIsReady;

    ServerGameMode->ChangePlayerInfo(GetPlayerState()->GetUniqueId(), PlayerInfo);
}

void ACCPlayerPawnGame::Server_EndPlayerTurn_Implementation()
{
    if (!ServerGameMode)
        return;

    Server_CleanSelectionData();
    Multicast_SetCurrentTurn(false);
    Server_CleanAllDices();
    ServerGameMode->StartNextTurn();
    bDicesSpawned = false;
    Client_ResetCameraToDefault();
}

void ACCPlayerPawnGame::Server_DebugEndPlayerTurn_Implementation()
{
    UE_LOG(LogTemp, Display, TEXT("Debug End turn from Pawn"));
    Server_EndPlayerTurn();
}

void ACCPlayerPawnGame::Server_CleanAllDices_Implementation()
{
    if (!ServerGameState)
        return;

    SelectedDiceActor = nullptr;
    TArray<ACCDice*> DicesArray = ServerGameState->GetSpawnedDices();

    if (DicesArray.Num() == 0)
        return;

    for (ACCDice* Dice : DicesArray)
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
        Client_VisualDeselectActor(SelectItemPawnComponent, SelectedPawnActor->GetMesh());
        SelectedPawnActor = nullptr;
    }
}

void ACCPlayerPawnGame::Multicast_SetCurrentTurn_Implementation(bool Turn)
{
    bCurrentTurn = Turn;
}

void ACCPlayerPawnGame::Client_EnableTurnButton_Implementation()
{
    if (OwningPlayerController)
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
    if (Component)
        Component->DeselectThisItem(Mesh);
}

void ACCPlayerPawnGame::Client_VisualSelectActor_Implementation(UCCSelectItem* Component, UMeshComponent* Mesh)
{
    if (Component)
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
        Client_VisualDeselectActor(SelectItemPawnComponent, SelectedPawnActor->GetMesh());

    SelectedPawnActor = HitPawn;
    Client_VisualSelectActor(SelectItemPawnComponent, SelectedPawnActor->GetMesh());
}

void ACCPlayerPawnGame::ClickOnBoard()
{
    if (!bCurrentTurn || bIsPawnMoving)
        return;

    if (!OwningPlayerController)
        return;

    FHitResult HitResult;
    if (OwningPlayerController->GetHitResultUnderCursor(ECC_Visibility, false, HitResult))
    {
        if (ACCDice* HitDice = Cast<ACCDice>(HitResult.GetActor()))
            Server_SelectDiceActor(HitDice);
        else if (ACCPawn* HitPawn = Cast<ACCPawn>(HitResult.GetActor()))
        {
            Server_SelectPawnActor(HitPawn);
            PawnClickedTimes++;
            GetWorld()->GetTimerManager().SetTimer(
                ResetDoubleClickPawnHandle, this, &ACCPlayerPawnGame::ResetPawnClickedTimes, 0.5f, false);
        }
    }
    if (bDicesSpawned)
        Server_CheckIfCanEnableEndTurn();

    Server_TrySwitchMovePawnButtonIsEnabled(true);
}

void ACCPlayerPawnGame::DoubleClickOnBoard()
{
    UE_LOG(LogTemp, Display, TEXT("Double "));
    if (PawnClickedTimes >= 2 && SelectedPawnActor)
    {
        UE_LOG(LogTemp, Display, TEXT("Double on pawn"));
        PawnClickedTimes = 0;
        PositionFromMove = GetActorLocation();
        PositionToMove = SelectedPawnActor->GetActorLocation();

        Client_MoveActorToPosition(CameraMovementToPawnCurve);
    }
}

void ACCPlayerPawnGame::ResetPawnClickedTimes()
{
    PawnClickedTimes = 0;
}

void ACCPlayerPawnGame::Server_MoveSelectedPawn_Implementation()
{
    if (!ServerGameState)
        return;

    if (!SelectedDiceActor || !SelectedPawnActor || bIsPawnMoving)
        return;

    PawnManagerComponent->MoveSelectedPawn(SelectedPawnActor, SelectedDiceActor->GetDiceSide());
    Multicast_HandlePawnMovementStarted();

    Client_VisualDeselectActor(SelectItemPawnComponent, SelectedPawnActor->GetMesh());

    ServerGameState->RemoveDice(SelectedDiceActor); // Exists for debug purpose
    if (SelectedDiceActor->bDestryWhenUsed)
        SelectedDiceActor->Destroy();

    if (ACCControllerGame* PlayerController = Cast<ACCControllerGame>(GetController()))
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
    Client_ResetCameraToDefault();
}

void ACCPlayerPawnGame::Server_CheckIsAnyMoveAvailable_Implementation()
{
    if (!ServerGameState)
        return;

    TArray<ACCPawn*> AllPawns = ServerGameState->GetAllPawns();
    TArray<ACCDice*> SpawnedDices = ServerGameState->GetSpawnedDices();
    bIsAnyPawnCanMove = false;

    if (SpawnedDices.Num() == 0 || AllPawns.Num() == 0)
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
    if (ServerGameMode)
        ServerGameMode->FinishGame(PlayerTagName);
}

void ACCPlayerPawnGame::Server_UpdateLobbySelection_Implementation()
{
    if (!ServerGameState)
        return;

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
    if (!ServerGameState)
        return;

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
    if (!ServerGameState)
        return;

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
    if (ServerGameMode)
        ServerGameMode->DisconnectPlayer(PlayerID);
}

void ACCPlayerPawnGame::Client_ResetCameraToDefault_Implementation()
{
    if (!SpringArmComponent || !CameraComponent || !CameraMovementToDefaultCurve || bIsCameraInDefaultState)
        return;

    // Create Timeline to move camera back to default position
    // Curve is set from 0 to 1
    DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

    bIsCameraMoving = true;
    InitialArmLength = SpringArmComponent->TargetArmLength;
    InitialArmRotation = SpringArmComponent->GetRelativeRotation();
    InitialActorRotation = GetActorRotation();
    PositionFromMove = GetActorLocation();
    InitalCameraRotation = CameraComponent->GetRelativeRotation();

    ProgressTimelineFunction.BindUFunction(this, FName("MoveCameraToDefaultPosition"));
    TimelineFinishedCallback.BindUFunction(this, FName("FinishCameraMovementToDefaultPosistion"));
    TimelineComponent->AddInterpFloat(CameraMovementToDefaultCurve, ProgressTimelineFunction, FName("Alpha"));
    TimelineComponent->SetTimelineFinishedFunc(TimelineFinishedCallback);
    TimelineComponent->SetLooping(false);
    TimelineComponent->PlayFromStart();
}

// Runs each Tick while timeline is active
void ACCPlayerPawnGame::MoveCameraToDefaultPosition(float Value)
{
    const float NewArmLength = FMath::Lerp(InitialArmLength, DefaultSpringArmLenght, Value);
    const FRotator NewActorRotation = FMath::Lerp(InitialActorRotation, DefaultActorRotation, Value);
    const FRotator NewCameraRotation = FMath::Lerp(InitalCameraRotation, FRotator(0.0f, 0.0f, 0.0f), Value);
    const FRotator NewArmRotation = FMath::Lerp(InitialArmRotation, DefaultSpringArmRotation, Value);
    const FVector NewLocation = FMath::Lerp(PositionFromMove, DefaultActorLocation, Value);

    SpringArmComponent->TargetArmLength = NewArmLength;
    SpringArmComponent->SetRelativeRotation(NewArmRotation);
    CameraComponent->SetRelativeRotation(NewCameraRotation);
    SetActorRotation(NewActorRotation);
    SetActorLocation(NewLocation);
}

void ACCPlayerPawnGame::FinishCameraMovementToDefaultPosistion()
{
    bIsCameraMoving = false;
    bIsCameraInDefaultState = true;
    bIsCameraFolowPawn = false;
}

void ACCPlayerPawnGame::Client_SetCameraInitPosition_Implementation(const FName Tag)
{
    FRotator NewRotation = GetActorRotation();
    if (UEnum::GetValueAsName(ETurnColors::Red) == Tag)
        NewRotation.Yaw = 90.0f;
    else if (UEnum::GetValueAsName(ETurnColors::Green) == Tag)
        NewRotation.Yaw = 180.0f;
    else if (UEnum::GetValueAsName(ETurnColors::Yellow) == Tag)
        NewRotation.Yaw = 270.0f;
    else if (UEnum::GetValueAsName(ETurnColors::Blue) == Tag)
        NewRotation.Yaw = 0.0f;

    DefaultActorRotation = NewRotation;
    SetActorRotation(DefaultActorRotation);
}

void ACCPlayerPawnGame::ZoomCamera(const FInputActionValue& Value)
{
    if (bIsCameraMoving)
        return;

    const float ZoomCameraValue = Value.Get<float>();

    if (bIsCameraFolowPawn)
    {
        ZoomCameraFromPawn(ZoomCameraValue);
        return;
    }

    // Hardcoded untill it will be needed to setup in blueprints
    const float MinZoomDistance = 2500.0f;
    const float CurrentZoomDistance = SpringArmComponent->TargetArmLength;
    const float SmallMultiplier = 0.5f;
    const float LargeMultiplier = 2.0f;
    const float MinPitchAngle = -30.0f;
    const float MaxPitchAngle = -70.0f;

    const float NormalizedDistance = (CurrentZoomDistance - MinZoomDistance) / (DefaultSpringArmLenght - MinZoomDistance);
    const float ZoomStepMultiplay = FMath::Lerp(SmallMultiplier, LargeMultiplier, NormalizedDistance);
    const float Step = -200.0f * ZoomStepMultiplay;

    float NewArmLength = CurrentZoomDistance + ZoomCameraValue * Step;

    // Setup camera rotation
    float NewNormalizedDistance = (NewArmLength - MinZoomDistance) / (DefaultSpringArmLenght - MinZoomDistance);
    FRotator CurrentRotation = SpringArmComponent->GetRelativeRotation();
    float NewPitchAngle = FMath::Lerp(MinPitchAngle, MaxPitchAngle, NewNormalizedDistance);
    CurrentRotation.Pitch = NewPitchAngle;

    if (NewArmLength > DefaultSpringArmLenght)
    {
        CurrentRotation.Pitch = MaxPitchAngle;
        NewArmLength = DefaultSpringArmLenght;
    }
    else if (NewArmLength < MinZoomDistance)
    {
        CurrentRotation.Pitch = MinPitchAngle;
        NewArmLength = MinZoomDistance;
    }

    bIsCameraInDefaultState = false;
    SpringArmComponent->TargetArmLength = NewArmLength;
    SpringArmComponent->SetRelativeRotation(CurrentRotation);
}

void ACCPlayerPawnGame::ZoomCameraFromPawn(float ZoomCameraValue)
{
    if (!CameraComponent)
        return;

    // Hardcoded untill it will be needed to setup in blueprints
    const float MinZoomDistance = 1500.0f;
    const float CurrentZoomDistance = SpringArmComponent->TargetArmLength;
    const float SmallMultiplier = 0.6f;
    const float LargeMultiplier = 1.8f;
    const float MinArmPitchAngle = -70.0f;
    const float MaxArmPitchAngle = -60.0f;
    const float MinCameraPitchAngle = 32.0f;
    const float MaxCameraPitchAngle = 20.0f;

    const float NormalizedDistance = (CurrentZoomDistance - MinZoomDistance) / (PawnSelectedSpringArmLenght - MinZoomDistance);
    const float ZoomStepMultiplay = FMath::Lerp(SmallMultiplier, LargeMultiplier, NormalizedDistance);
    const float Step = -125.0f * ZoomStepMultiplay;

    float NewArmLength = CurrentZoomDistance + ZoomCameraValue * Step;

    // Setup camera rotation
    const float NewNormalizedDistance = (NewArmLength - MinZoomDistance) / (PawnSelectedSpringArmLenght - MinZoomDistance);
    FRotator CurrentArmRotation = SpringArmComponent->GetRelativeRotation();
    FRotator CurrentCameraRotation = CameraComponent->GetRelativeRotation();
    const float NewArmPitchAngle = FMath::Lerp(MinArmPitchAngle, MaxArmPitchAngle, NewNormalizedDistance);
    const float NewCameraPitchAngle = FMath::Lerp(MinCameraPitchAngle, MaxCameraPitchAngle, NewNormalizedDistance);
    CurrentArmRotation.Pitch = NewArmPitchAngle;
    CurrentCameraRotation.Pitch = NewCameraPitchAngle;

    if (NewArmLength > PawnSelectedSpringArmLenght)
    {
        CurrentCameraRotation.Pitch = MaxCameraPitchAngle;
        CurrentArmRotation.Pitch = MaxArmPitchAngle;
        NewArmLength = PawnSelectedSpringArmLenght;
    }
    else if (NewArmLength < MinZoomDistance)
    {
        CurrentCameraRotation.Pitch = MinCameraPitchAngle;
        CurrentArmRotation.Pitch = MinArmPitchAngle;
        NewArmLength = MinZoomDistance;
    }

    UE_LOG(LogTemp, Display, TEXT("Camera Rotation:%s, Arm Rotation:%s, Arm:%f"), *CurrentCameraRotation.ToString(),
        *CurrentArmRotation.ToString(), NewArmLength);
    bIsCameraInDefaultState = false;

    SpringArmComponent->TargetArmLength = NewArmLength;
    SpringArmComponent->SetRelativeRotation(CurrentArmRotation);
    CameraComponent->SetRelativeRotation(CurrentCameraRotation);
}

void ACCPlayerPawnGame::ResetCameraByClick() 
{
    Client_ResetCameraToDefault();
}

void ACCPlayerPawnGame::RotateCamera(const FInputActionValue& Value)
{
    if (bIsCameraMoving)
        return;

    float RotateCameraValue = Value.Get<float>();

    FRotator NewRotation = GetActorRotation();
    NewRotation.Yaw += RotateCameraValue * 2;

    bIsCameraInDefaultState = false;
    SetActorRotation(NewRotation);
}

void ACCPlayerPawnGame::Client_MoveActorToPosition_Implementation(UCurveFloat* CurveToUse)
{
    if (!SpringArmComponent || !CameraComponent || !CurveToUse)
        return;

    bIsCameraMoving = true;
    bIsCameraInDefaultState = false;
    InitialArmLength = SpringArmComponent->TargetArmLength;
    InitialArmRotation = SpringArmComponent->GetRelativeRotation();
    InitalCameraRotation = CameraComponent->GetRelativeRotation();
    PawnSpringArmRotationTarget = SpringArmComponent->GetRelativeRotation();
    PawnSpringArmRotationTarget.Pitch = PawnSpringArmRotationPitch;

    ProgressTimelineFunction.BindUFunction(this, FName("MoveActorToSelectedPosition"));
    TimelineFinishedCallback.BindUFunction(this, FName("FinishActorMovementToSelectedPosistion"));
    TimelineComponent->AddInterpFloat(CurveToUse, ProgressTimelineFunction, FName("Alpha"));
    TimelineComponent->SetTimelineFinishedFunc(TimelineFinishedCallback);
    TimelineComponent->SetLooping(false);
    TimelineComponent->PlayFromStart();
}

void ACCPlayerPawnGame::MoveActorToSelectedPosition(float Value)
{
    const FVector NewLocation = FMath::Lerp(PositionFromMove, PositionToMove, Value);
    SetActorLocation(NewLocation);

    if (bIsCameraFolowPawn)
        return;

    const float NewArmLength = FMath::Lerp(InitialArmLength, PawnSelectedSpringArmLenght, Value);
    const FRotator NewCameraRotation = FMath::Lerp(InitalCameraRotation, PawnCameraRotation, Value);
    const FRotator NewArmRotation = FMath::Lerp(InitialArmRotation, PawnSpringArmRotationTarget, Value);

    SpringArmComponent->TargetArmLength = NewArmLength;
    CameraComponent->SetRelativeRotation(NewCameraRotation);
    SpringArmComponent->SetRelativeRotation(NewArmRotation);
}

void ACCPlayerPawnGame::FinishActorMovementToSelectedPosistion()
{
    bIsCameraFolowPawn = true;
    bIsCameraMoving = false;
    PositionToMove = FVector();
    PositionFromMove = FVector();

    AttachToComponent(SelectedPawnActor->PawnPositonComponent, FAttachmentTransformRules::KeepWorldTransform);
}