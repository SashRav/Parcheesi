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
    PawnMovingData.FocalLength = MinPawnFocalLength;

    DefaultMovingData.ActorLocation = OwningActor->GetActorLocation();
    DefaultMovingData.CameraRotation = DefaultCameraRotation;
    DefaultMovingData.SpringArmRotation = DefaultSpringArmRotation;
    DefaultMovingData.FocalLength = MinFocalLength;

    SpringArmComponent->SetRelativeRotation(DefaultMovingData.SpringArmRotation);
    SpringArmComponent->TargetArmLength = MaxSpringArmLength;
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
    StartFocalLength = MinFocalLength;
}

void UCCCameraControlComponent::ZoomCamera(const FInputActionValue& Value)
{
    if (bIsCameraMovingToPoint)
        return;

    const float ZoomCameraValue = Value.Get<float>();
    const float CurrentFocalLength = StartFocalLength;

    float LargeMultiplier = 0.5f;
    float SmallMultiplier = 2.0f;
    float MinArmPitchAngle = -30.0f;
    float MaxArmPitchAngle = -70.0f;
    float MinCameraPitchAngle = 0.0f;
    float MaxCameraPitchAngle = 0.0f;
    float StepValue = 200.0f;
    float FoculLengthStep = 2.0f;

    float MinTargetFocalLength = MinFocalLength;
    float MaxTargetFocalLength = MaxFocalLength;

    if (bIsCameraFolowPawn)
    {
        MinTargetFocalLength = MinPawnFocalLength;
        MaxTargetFocalLength = MaxPawnFocalLength;

        StepValue = 125.0f;
        SmallMultiplier = 0.6f;
        LargeMultiplier = 1.8f;
        MinArmPitchAngle = -70.0f;
        MaxArmPitchAngle = -60.0f;
        MinCameraPitchAngle = 32.0f;
        MaxCameraPitchAngle = 20.0f;
    }

    bShouldMoveOnlyFocalLength = true;

    const float NormilizedFocalLength = (CurrentFocalLength - MaxTargetFocalLength) / (MinTargetFocalLength - MaxTargetFocalLength);
    const float FocalLengthStepMultiplay = FMath::Lerp(SmallMultiplier, LargeMultiplier, NormilizedFocalLength);
    const float FocalLengthStep = FoculLengthStep * FocalLengthStepMultiplay;
    const float NewFocalLength = CurrentFocalLength + FocalLengthStep * ZoomCameraValue;
    const float TargetFocalLength = FMath::Clamp(NewFocalLength, MinTargetFocalLength, MaxTargetFocalLength);

    StartFocalLength = TargetFocalLength;

    bIsCameraInDefaultState = false;

    FCameraMovemntData ZoomData;
    ZoomData.FocalLength = TargetFocalLength;
    StartMoveCameraToTargetPositon(DefaultCameraMovementCurve, ZoomData);
}

void UCCCameraControlComponent::MoveCameraOnLevel(const FInputActionValue& Value)
{
    // Movement Limited by SphereCollision in OwningActor and Blocking volumes on the level

    if (bIsCameraMovingToPoint)
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

    const float CurrentFocalLength = StartFocalLength;

    const float NormalizedDistance = (StartFocalLength - MaxFocalLength) / (MinFocalLength - MaxFocalLength);
    const float MovementMultiplier = FMath::Lerp(0.8, 1.0f, NormalizedDistance);

    OwningActor->AddMovementInput(ForwardDirection, MovementVector.Y * MovementMultiplier);
    OwningActor->AddMovementInput(RightDirection, MovementVector.X * MovementMultiplier);

    bIsCameraInDefaultState = false;
}

void UCCCameraControlComponent::RotateCamera(const FInputActionInstance& Value)
{
    if (bIsCameraMovingToPoint && !bIsPawnChangedWhenFocused) // Can rotate camera when focusing another pawn
        return;

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
    bIsCameraMovingToPoint = true;
    TargetSocketOffset = DefaultSocketOffset;
    StartFocalLength = MinFocalLength;

    bShouldMoveOnlyFocalLength = false;
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
    bShouldMoveOnlyFocalLength = false;
    bIsCameraMovingToPoint = true;

    if (bIsCameraFolowPawn)
        bIsPawnChangedWhenFocused = true;

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

    InitalMovingData.FocalLength = CineCameraComponent->CurrentFocalLength;
    if (!bShouldMoveOnlyFocalLength)
    {

        InitalMovingData.SpringArmRotation = SpringArmComponent->GetRelativeRotation();
        InitalMovingData.CameraRotation = CineCameraComponent->GetRelativeRotation();
        InitalMovingData.ActorRotation = OwningActor->GetActorRotation();
        InitalMovingData.ActorLocation = OwningActor->GetActorLocation();
        InitalSocketOffset = SpringArmComponent->SocketOffset;
    }

    ProgressTimelineFunction.BindUFunction(this, FName("MoveCameraToTargetPosition"));
    TimelineFinishedCallback.BindUFunction(this, FName("FinishCameraMovementToTargetPosistion"));
    TimelineComponent->AddInterpFloat(CurveToUse, ProgressTimelineFunction, FName("Alpha"));
    TimelineComponent->SetTimelineFinishedFunc(TimelineFinishedCallback);
    TimelineComponent->SetLooping(false);
    TimelineComponent->PlayFromStart();
}

void UCCCameraControlComponent::MoveCameraToTargetPosition(float Value)
{
    if (bIsPawnChangedWhenFocused)
    {
        const FVector NewActorLocation = FMath::Lerp(InitalMovingData.ActorLocation, TargetMovingData.ActorLocation, Value);
        OwningActor->SetActorLocation(NewActorLocation);
        return;
    }
    if (bShouldMoveOnlyFocalLength)
    {
        const float NewFocalLength = FMath::Lerp(InitalMovingData.FocalLength, TargetMovingData.FocalLength, Value);
        CineCameraComponent->CurrentFocalLength = NewFocalLength;
        return;
    }

    const float NewFocalLength = FMath::Lerp(InitalMovingData.FocalLength, TargetMovingData.FocalLength, Value);
    const FVector NewActorLocation = FMath::Lerp(InitalMovingData.ActorLocation, TargetMovingData.ActorLocation, Value);
    const FVector NewSocketOffset = FMath::Lerp(InitalSocketOffset, TargetSocketOffset, Value);
    const FRotator NewCameraRotation = FMath::Lerp(InitalMovingData.CameraRotation, TargetMovingData.CameraRotation, Value);
    const FRotator NewArmRotation = FMath::Lerp(InitalMovingData.SpringArmRotation, TargetMovingData.SpringArmRotation, Value);
    const FRotator NewActorRotation = FMath::Lerp(InitalMovingData.ActorRotation, TargetMovingData.ActorRotation, Value);

    SpringArmComponent->SetRelativeRotation(NewArmRotation);
    SpringArmComponent->SocketOffset = NewSocketOffset;
    OwningActor->SetActorLocation(NewActorLocation);
    CineCameraComponent->CurrentFocalLength = NewFocalLength;
    CineCameraComponent->SetRelativeRotation(NewCameraRotation);
    OwningActor->SetActorRotation(NewActorRotation);
}

void UCCCameraControlComponent::FinishCameraMovementToTargetPosistion()
{
    bIsCameraMoving = false;
    bShouldMoveOnlyFocalLength = false;
    bIsCameraMovingToPoint = false;
    bIsPawnChangedWhenFocused = false;

    if (bIsCameraFolowPawn)
    {
        OwningActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
        bIsCameraFolowPawn = false;
    }

    if (bShouldActorBeAttach && OwningActor->GetSelectedPawnSceneComponent())
    {
        OwningActor->AttachToComponent(OwningActor->GetSelectedPawnSceneComponent(), FAttachmentTransformRules::KeepWorldTransform);
        bIsCameraFolowPawn = true;
        StartFocalLength = MinPawnFocalLength;
    }
}