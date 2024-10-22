// Copyright Cats and Cubes. All Rights Reserved.

#include "Components/CCCameraControlComponent.h"
#include "Components/TimelineComponent.h"
#include "CineCameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Framework/CCPlayerPawnGame.h"
#include "InputActionValue.h"
#include "CCCoreTypes.h"
#include "EnhancedInputComponent.h"

UCCCameraControlComponent::UCCCameraControlComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UCCCameraControlComponent::BeginPlay()
{
    Super::BeginPlay();

    check(CameraMovementToDefaultCurve);
    check(CameraMovementToPawnCurve);
    check(DefaultCameraMovementCurve);
    check(EndCameraMovementCurve);

    OwningActor = Cast<ACCPlayerPawnGame>(GetOwner());

    PawnMovingData.CameraRotation = DefaultPawnCameraRotation;
    PawnMovingData.SpringArmRotation = DefaultPawnSpringArmRotation;
    PawnMovingData.SpringArmLength = MaxPawnSpringArmLength;

    DefaultMovingData.ActorLocation = OwningActor->GetActorLocation();
    DefaultMovingData.CameraRotation = DefaultCameraRotation;
    DefaultMovingData.SpringArmRotation = DefaultSpringArmRotation;
    DefaultMovingData.SpringArmLength = MaxSpringArmLength;

    SpringArmComponent->SetRelativeRotation(DefaultMovingData.SpringArmRotation);
    SpringArmComponent->TargetArmLength = DefaultMovingData.SpringArmLength;
    SpringArmComponent->bEnableCameraRotationLag = true;
    SpringArmComponent->bEnableCameraLag = true;
    SpringArmComponent->CameraRotationLagSpeed = DefaultRotationLagSpeed;
    SpringArmComponent->CameraLagSpeed = DefaultLagSpeed;
    SpringArmComponent->SocketOffset = DefaultSocketOffset;

    CineCameraComponent->bConstrainAspectRatio = false;
    CineCameraComponent->CurrentFocalLength = MinFocalLength;
}

void UCCCameraControlComponent::SetCameraInitPosition(const FName Tag)
{
    FRotator NewRotation = OwningActor->GetActorRotation();
    if (UEnum::GetValueAsName(ETurnColors::Red) == Tag)
        NewRotation.Yaw = 90.0f;
    else if (UEnum::GetValueAsName(ETurnColors::Green) == Tag)
        NewRotation.Yaw = 180.0f;
    else if (UEnum::GetValueAsName(ETurnColors::Yellow) == Tag)
        NewRotation.Yaw = 270.0f;
    else if (UEnum::GetValueAsName(ETurnColors::Blue) == Tag)
        NewRotation.Yaw = 0.0f;

    OwningActor->SetActorRotation(NewRotation);

    DefaultMovingData.ActorRotation = NewRotation;
    StartArmLenght = SpringArmComponent->TargetArmLength;
    StartFocalLength = MinFocalLength;
}

void UCCCameraControlComponent::ZoomCamera(const FInputActionValue& Value)
{
    const float ZoomCameraValue = Value.Get<float>();
    const float CurrentZoomDistance = StartArmLenght;
    const float CurrentFocalLength = StartFocalLength;

    float SpringArmMaxLength = DefaultMovingData.SpringArmLength;
    float MinZoomDistance = MinSpringArmLength;
    float LargeMultiplier = 0.5f;
    float SmallMultiplier = 2.0f;
    // float MinArmPitchAngle = -30.0f;
    // float MaxArmPitchAngle = -70.0f;
    // float MinCameraPitchAngle = 0.0f;
    // float MaxCameraPitchAngle = 0.0f;
    float StepValue = 200.0f;
    float FoculLengthStep = 2.0f;

    if (bIsCameraFolowPawn)
    {
        SpringArmMaxLength = PawnMovingData.SpringArmLength;
        MinZoomDistance = MinPawnSpringArmLength;
        StepValue = 125.0f;
        SmallMultiplier = 0.6f;
        LargeMultiplier = 1.8f;
        // MinArmPitchAngle = -70.0f;
        // MaxArmPitchAngle = -60.0f;
        // MinCameraPitchAngle = 32.0f;
        // MaxCameraPitchAngle = 20.0f;
    }

    const float NormalizedDistance = (CurrentZoomDistance - MinZoomDistance) / (SpringArmMaxLength - MinZoomDistance);
    const float ZoomStepMultiplay = FMath::Lerp(SmallMultiplier, LargeMultiplier, NormalizedDistance);
    const float Step = -StepValue * ZoomStepMultiplay;

    float NewArmLength = CurrentZoomDistance + ZoomCameraValue * Step;

    NewArmLength = FMath::Clamp(NewArmLength, MinZoomDistance, SpringArmMaxLength);
    bIsCameraInDefaultState = false;

    StartArmLenght = NewArmLength;

    bShouldMoveOnlyArm = true;

    const float NormilizedFocalLength = (CurrentFocalLength - MaxFocalLength) / (MinFocalLength - MaxFocalLength);
    const float FocalLengthStepMultiplay = FMath::Lerp(SmallMultiplier, LargeMultiplier, NormilizedFocalLength);
    const float FocalLengthStep = FoculLengthStep * FocalLengthStepMultiplay;
    const float NewFocalLength = CurrentFocalLength + FocalLengthStep * ZoomCameraValue;
    TargetFocalLength = FMath::Clamp(NewFocalLength, MinFocalLength, MaxFocalLength);
    UE_LOG(LogTemp, Display, TEXT("Foc: %f"), TargetFocalLength);

    StartFocalLength = TargetFocalLength;

    // CineCameraComponent->CurrentFocalLength = LimitedFocalLength;
    FCameraMovemntData ZoomData;
    // ZoomData.SpringArmLength = NewArmLength;
    StartMoveCameraToTargetPositon(DefaultCameraMovementCurve, ZoomData);
}

void UCCCameraControlComponent::MoveCameraOnLevel(const FInputActionValue& Value)
{
    // Movement Limited by SphereCollision in OwningActor and Blocking volumes on the level

    if (bIsCameraMovingToDefault)
        return;

    if (bIsCameraFolowPawn)
    {
        ResetCameraToDefault(); // Should be tested and discussed
        return;
    }

    const FVector2D MovementVector = Value.Get<FVector2D>();

    const FRotator Rotation = SpringArmComponent->GetComponentRotation();
    const FRotator YawRotation(0, Rotation.Yaw, 0);

    const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    const float CurrentZoomDistance = StartArmLenght;

    const float NormalizedDistance =
        (StartArmLenght - MinSpringArmLength) / (MaxSpringArmLength - MinSpringArmLength); // Hardcoded max zoom values
    const float MovementMultiplier = FMath::Lerp(0.8, 1.0f, NormalizedDistance);

    OwningActor->AddMovementInput(ForwardDirection, MovementVector.Y * MovementMultiplier);
    OwningActor->AddMovementInput(RightDirection, MovementVector.X * MovementMultiplier);

    bIsCameraInDefaultState = false;
}

void UCCCameraControlComponent::RotateCamera(const FInputActionInstance& Value)
{
    const float StartTime = 0.0f;
    const float EndTime = 1.0f;
    const float StartMultiplier = 1.0f;
    const float EndMultiplier = 0.3f;

    FRotator NewRotation = SpringArmComponent->GetRelativeRotation();
    const float RotateCameraValue = Value.GetValue().Get<float>();
    NewRotation.Yaw += RotateCameraValue * 2.0f;

    float CurrentRunTime = FMath::Clamp(Value.GetElapsedTime(), StartTime, EndTime);
    float LagMultiplier = StartMultiplier - ((StartMultiplier - EndMultiplier) / (EndTime - StartTime)) * (CurrentRunTime - StartTime);
    SpringArmComponent->CameraRotationLagSpeed = 10.0f * LagMultiplier;

    bIsCameraInDefaultState = false;
    SpringArmComponent->SetRelativeRotation(NewRotation);
}

void UCCCameraControlComponent::ResetCameraToDefault()
{
    if (bIsCameraInDefaultState)
        return;

    bShouldActorBeAttach = false;
    bIsCameraInDefaultState = true;
    bIsCameraMovingToDefault = true;
    TargetSocketOffset = DefaultSocketOffset;
    TargetFocalLength = MinFocalLength;
    StartFocalLength = MinFocalLength;

    bShouldMoveOnlyArm = false;
    StartMoveCameraToTargetPositon(CameraMovementToDefaultCurve, DefaultMovingData);
}

void UCCCameraControlComponent::TryToResetCameraAfterPawnMove()
{
    if (!bIsCameraFolowPawn)
        return;

    ResetCameraToDefault();
}

void UCCCameraControlComponent::MoveCameraToPawn(FVector TargetLocation)
{
    PawnMovingData.ActorLocation = TargetLocation;
    PawnMovingData.ActorRotation = OwningActor->GetActorRotation();
    bShouldActorBeAttach = true;
    bIsCameraInDefaultState = false;
    TargetSocketOffset = PawnSocketOffset;
    TargetFocalLength = MinPawnFocalLength;
    bShouldMoveOnlyArm = false;
    StartMoveCameraToTargetPositon(CameraMovementToPawnCurve, PawnMovingData);
}

void UCCCameraControlComponent::ResetCameraLag()
{
    SpringArmComponent->CameraRotationLagSpeed = DefaultRotationLagSpeed;
}

void UCCCameraControlComponent::StartMoveCameraToTargetPositon(UCurveFloat* CurveToUse, FCameraMovemntData TargetData)
{
    if (!SpringArmComponent || !CineCameraComponent || !CurveToUse)
        return;

    TargetMovingData = TargetData;

    bIsCameraMoving = true;

    InitalMovingData.SpringArmLength = SpringArmComponent->TargetArmLength;
    if (!bShouldMoveOnlyArm)
    {
        InitalMovingData.SpringArmRotation = SpringArmComponent->GetRelativeRotation();
        InitalMovingData.CameraRotation = CineCameraComponent->GetRelativeRotation();
        InitalMovingData.ActorRotation = OwningActor->GetActorRotation();
        InitalMovingData.ActorLocation = OwningActor->GetActorLocation();
        InitalSocketOffset = SpringArmComponent->SocketOffset;
    }
    InitialFocalLength = CineCameraComponent->CurrentFocalLength;
    UE_LOG(LogTemp, Display, TEXT("Init Foc: %f"), InitialFocalLength);

    ProgressTimelineFunction.BindUFunction(this, FName("MoveCameraToTargetPosition"));
    TimelineFinishedCallback.BindUFunction(this, FName("FinishCameraMovementToTargetPosistion"));
    TimelineComponent->AddInterpFloat(CurveToUse, ProgressTimelineFunction, FName("Alpha"));
    TimelineComponent->SetTimelineFinishedFunc(TimelineFinishedCallback);
    TimelineComponent->SetLooping(false);
    TimelineComponent->PlayFromStart();
}

void UCCCameraControlComponent::MoveCameraToTargetPosition(float Value)
{
    /*const float NewArmLength = FMath::Lerp(InitalMovingData.SpringArmLength, TargetMovingData.SpringArmLength, Value);
    SpringArmComponent->TargetArmLength = NewArmLength;
    */
    const float NewFocalLength = FMath::Lerp(InitialFocalLength, TargetFocalLength, Value);
    CineCameraComponent->CurrentFocalLength = NewFocalLength;

    if (bShouldMoveOnlyArm)
        return;

    const FVector NewSocketOffset = FMath::Lerp(InitalSocketOffset, TargetSocketOffset, Value);
    const FVector NewActorLocation = FMath::Lerp(InitalMovingData.ActorLocation, TargetMovingData.ActorLocation, Value);
    const FRotator NewCameraRotation = FMath::Lerp(InitalMovingData.CameraRotation, TargetMovingData.CameraRotation, Value);
    const FRotator NewArmRotation = FMath::Lerp(InitalMovingData.SpringArmRotation, TargetMovingData.SpringArmRotation, Value);
    const FRotator NewActorRotation = FMath::Lerp(InitalMovingData.ActorRotation, TargetMovingData.ActorRotation, Value);

    SpringArmComponent->SetRelativeRotation(NewArmRotation);
    SpringArmComponent->SocketOffset = NewSocketOffset;

    CineCameraComponent->SetRelativeRotation(NewCameraRotation);
    OwningActor->SetActorRotation(NewActorRotation);
    OwningActor->SetActorLocation(NewActorLocation);
}

void UCCCameraControlComponent::FinishCameraMovementToTargetPosistion()
{
    bIsCameraMoving = false;
    bShouldMoveOnlyArm = false;
    bIsCameraMovingToDefault = false;

    if (bIsCameraFolowPawn)
    {
        OwningActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
        bIsCameraFolowPawn = false;
    }

    if (bShouldActorBeAttach && OwningActor->GetSelectedPawnSceneComponent())
    {
        OwningActor->AttachToComponent(OwningActor->GetSelectedPawnSceneComponent(), FAttachmentTransformRules::KeepWorldTransform);
        bIsCameraFolowPawn = true;
        StartArmLenght = PawnMovingData.SpringArmLength;
        StartFocalLength = MinPawnFocalLength;
    }
}