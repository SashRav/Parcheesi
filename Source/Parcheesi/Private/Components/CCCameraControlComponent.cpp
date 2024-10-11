// Copyright Cats and Cubes. All Rights Reserved.

#include "Components/CCCameraControlComponent.h"
#include "Components/TimelineComponent.h"
#include "Camera/CameraComponent.h"
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

    PawnMovingData.CameraRotation = FRotator(20.0f, 0.0f, 0.0f);
    PawnMovingData.SpringArmRotation = FRotator(-60.0f, 0.0f, 0.0f);
    PawnMovingData.SpringArmLength = 3000.0f;

    DefaultMovingData.ActorLocation = OwningActor->GetActorLocation();
    DefaultMovingData.CameraRotation = FRotator(0.0f, 0.0f, 0.0f);
    DefaultMovingData.SpringArmRotation = FRotator(-70.0f, 0.0f, 0.0f);
    DefaultMovingData.SpringArmLength = 5700.0f;

    SpringArmComponent->SetRelativeRotation(DefaultMovingData.SpringArmRotation);
    SpringArmComponent->TargetArmLength = DefaultMovingData.SpringArmLength;
    SpringArmComponent->bEnableCameraRotationLag = true;
    SpringArmComponent->CameraRotationLagSpeed = DefaultRotationLagSpeed;
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
}

void UCCCameraControlComponent::ZoomCamera(const FInputActionValue& Value)
{
    const float ZoomCameraValue = Value.Get<float>();
    const float CurrentZoomDistance = StartArmLenght;

    float SpringArmMaxLength = DefaultMovingData.SpringArmLength;
    float MinZoomDistance = 2500.0f;
    float LargeMultiplier = 0.5f;
    float SmallMultiplier = 2.0f;
    float MinArmPitchAngle = -30.0f;
    float MaxArmPitchAngle = -70.0f;
    float MinCameraPitchAngle = 0.0f;
    float MaxCameraPitchAngle = 0.0f;
    float StepValue = 300.0f;

    if (bIsCameraFolowPawn)
    {
        MinZoomDistance = 1500.0f;
        StepValue = 200.0f;
        SmallMultiplier = 0.6f;
        LargeMultiplier = 1.8f;
        MinArmPitchAngle = -70.0f;
        MaxArmPitchAngle = -60.0f;
        MinCameraPitchAngle = 32.0f;
        MaxCameraPitchAngle = 20.0f;
        SpringArmMaxLength = PawnMovingData.SpringArmLength;
    }

    const float NormalizedDistance = (CurrentZoomDistance - MinZoomDistance) / (SpringArmMaxLength - MinZoomDistance);
    const float ZoomStepMultiplay = FMath::Lerp(SmallMultiplier, LargeMultiplier, NormalizedDistance);
    const float Step = -StepValue * ZoomStepMultiplay;

    float NewArmLength = CurrentZoomDistance + ZoomCameraValue * Step;

    NewArmLength = FMath::Clamp(NewArmLength, MinZoomDistance, SpringArmMaxLength);
    bIsCameraInDefaultState = false;

    StartArmLenght = NewArmLength;

    FCameraMovemntData ZoomData;
    ZoomData.SpringArmLength = NewArmLength;

    bShouldMoveOnlyArm = true;
    StartMoveCameraToTargetPositon(DefaultCameraMovementCurve, ZoomData);
}

void UCCCameraControlComponent::MoveCameraOnLevel(const FInputActionValue& Value)
{
    UE_LOG(LogTemp, Display, TEXT("Camera Is Moving: %s"), *Value.ToString());
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
    TargetSocketOffset = FVector(0.0f, 0.0f, 0.0f);

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
    bShouldMoveOnlyArm = false;
    StartMoveCameraToTargetPositon(CameraMovementToPawnCurve, PawnMovingData);
}

void UCCCameraControlComponent::ResetCameraLag()
{
    SpringArmComponent->CameraRotationLagSpeed = DefaultRotationLagSpeed;
}

void UCCCameraControlComponent::StartMoveCameraToTargetPositon(UCurveFloat* CurveToUse, FCameraMovemntData TargetData)
{
    if (!SpringArmComponent || !CameraComponent || !CurveToUse)
        return;

    TargetMovingData = TargetData;

    bIsCameraMoving = true;

    InitalMovingData.SpringArmLength = SpringArmComponent->TargetArmLength;
    if (!bShouldMoveOnlyArm)
    {
        InitalMovingData.SpringArmRotation = SpringArmComponent->GetRelativeRotation();
        InitalMovingData.CameraRotation = CameraComponent->GetRelativeRotation();
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
    const float NewArmLength = FMath::Lerp(InitalMovingData.SpringArmLength, TargetMovingData.SpringArmLength, Value);
    SpringArmComponent->TargetArmLength = NewArmLength;

    if (bShouldMoveOnlyArm)
        return;

    const FVector NewSocketOffset = FMath::Lerp(InitalSocketOffset, TargetSocketOffset, Value);
    const FVector NewActorLocation = FMath::Lerp(InitalMovingData.ActorLocation, TargetMovingData.ActorLocation, Value);
    const FRotator NewCameraRotation = FMath::Lerp(InitalMovingData.CameraRotation, TargetMovingData.CameraRotation, Value);
    const FRotator NewArmRotation = FMath::Lerp(InitalMovingData.SpringArmRotation, TargetMovingData.SpringArmRotation, Value);
    const FRotator NewActorRotation = FMath::Lerp(InitalMovingData.ActorRotation, TargetMovingData.ActorRotation, Value);

    SpringArmComponent->SetRelativeRotation(NewArmRotation);
    SpringArmComponent->SocketOffset = NewSocketOffset;

    CameraComponent->SetRelativeRotation(NewCameraRotation);
    OwningActor->SetActorRotation(NewActorRotation);
    OwningActor->SetActorLocation(NewActorLocation);
}

void UCCCameraControlComponent::FinishCameraMovementToTargetPosistion()
{
    bIsCameraMoving = false;
    bShouldMoveOnlyArm = false;

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
    }
}