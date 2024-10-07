// Copyright Cats and Cubes. All Rights Reserved.

#include "Components/CCCameraControlComponent.h"
#include "Components/TimelineComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Framework/CCPlayerPawnGame.h"
#include "InputActionValue.h"
#include "CCCoreTypes.h"

UCCCameraControlComponent::UCCCameraControlComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UCCCameraControlComponent::BeginPlay()
{
    Super::BeginPlay();

    check(CameraMovementToDefaultCurve);
    check(CameraMovementToPawnCurve);

    OwningActor = Cast<ACCPlayerPawnGame>(GetOwner());

    PawnMovingData.CameraRotation = FRotator(20.0f, 0.0f, 0.0f);
    PawnMovingData.SpringArmRotation = FRotator(-60.0f, 0.0f, 0.0f);
    PawnMovingData.SpringArmLength = 3000.0f;

    DefaultMovingData.ActorLocation = OwningActor->GetActorLocation();
    DefaultMovingData.CameraRotation = FRotator(0.0f, 0.0f, 0.0f);
    DefaultMovingData.SpringArmRotation = FRotator(-70.0f, 0.0f, 0.0f);
    DefaultMovingData.SpringArmLength = 5700.0f;

    SpringArmComponent->SocketOffset = FVector(0.0f, 0.0f, -700.0f);
    SpringArmComponent->SetRelativeRotation(DefaultMovingData.SpringArmRotation);
    SpringArmComponent->TargetArmLength = DefaultMovingData.SpringArmLength;
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
}

void UCCCameraControlComponent::ZoomCamera(const FInputActionValue& Value)
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

    const float NormalizedDistance = (CurrentZoomDistance - MinZoomDistance) / (DefaultMovingData.SpringArmLength - MinZoomDistance);
    const float ZoomStepMultiplay = FMath::Lerp(SmallMultiplier, LargeMultiplier, NormalizedDistance);
    const float Step = -200.0f * ZoomStepMultiplay;

    float NewArmLength = CurrentZoomDistance + ZoomCameraValue * Step;

    // Setup camera rotation
    float NewNormalizedDistance = (NewArmLength - MinZoomDistance) / (DefaultMovingData.SpringArmLength - MinZoomDistance);
    FRotator CurrentRotation = SpringArmComponent->GetRelativeRotation();
    float NewPitchAngle = FMath::Lerp(MinPitchAngle, MaxPitchAngle, NewNormalizedDistance);
    CurrentRotation.Pitch = NewPitchAngle;

    if (NewArmLength > DefaultMovingData.SpringArmLength)
    {
        CurrentRotation.Pitch = MaxPitchAngle;
        NewArmLength = DefaultMovingData.SpringArmLength;
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

void UCCCameraControlComponent::RotateCamera(const FInputActionValue& Value)
{
    if (bIsCameraMoving)
        return;

    float RotateCameraValue = Value.Get<float>();

    FRotator NewRotation = OwningActor->GetActorRotation();
    NewRotation.Yaw += RotateCameraValue * 2;

    bIsCameraInDefaultState = false;
    OwningActor->SetActorRotation(NewRotation);
}

void UCCCameraControlComponent::MoveCameraOnLevel(const FInputActionValue& Value) 
{
    UE_LOG(LogTemp, Display, TEXT("Camera Is Moving: %s"), *Value.ToString());
}

void UCCCameraControlComponent::ZoomCameraFromPawn(float ZoomCameraValue)
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

    const float NormalizedDistance = (CurrentZoomDistance - MinZoomDistance) / (PawnMovingData.SpringArmLength - MinZoomDistance);
    const float ZoomStepMultiplay = FMath::Lerp(SmallMultiplier, LargeMultiplier, NormalizedDistance);
    const float Step = -125.0f * ZoomStepMultiplay;

    float NewArmLength = CurrentZoomDistance + ZoomCameraValue * Step;

    // Setup camera rotation
    const float NewNormalizedDistance = (NewArmLength - MinZoomDistance) / (PawnMovingData.SpringArmLength - MinZoomDistance);
    FRotator CurrentArmRotation = SpringArmComponent->GetRelativeRotation();
    FRotator CurrentCameraRotation = CameraComponent->GetRelativeRotation();
    const float NewArmPitchAngle = FMath::Lerp(MinArmPitchAngle, MaxArmPitchAngle, NewNormalizedDistance);
    const float NewCameraPitchAngle = FMath::Lerp(MinCameraPitchAngle, MaxCameraPitchAngle, NewNormalizedDistance);
    CurrentArmRotation.Pitch = NewArmPitchAngle;
    CurrentCameraRotation.Pitch = NewCameraPitchAngle;

    if (NewArmLength > PawnMovingData.SpringArmLength)
    {
        CurrentCameraRotation.Pitch = MaxCameraPitchAngle;
        CurrentArmRotation.Pitch = MaxArmPitchAngle;
        NewArmLength = PawnMovingData.SpringArmLength;
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

void UCCCameraControlComponent::ResetCameraToDefault()
{
    if (bIsCameraInDefaultState)
        return;

    bShouldActorBeAttach = false;
    bIsCameraInDefaultState = true;
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
    StartMoveCameraToTargetPositon(CameraMovementToPawnCurve, PawnMovingData);
}

void UCCCameraControlComponent::StartMoveCameraToTargetPositon(UCurveFloat* CurveToUse, FCameraMovemntData TargetData)
{
    if (!SpringArmComponent || !CameraComponent || !CurveToUse)
        return;

    TargetMovingData = TargetData;

    bIsCameraMoving = true;

    InitalMovingData.SpringArmLength = SpringArmComponent->TargetArmLength;
    InitalMovingData.SpringArmRotation = SpringArmComponent->GetRelativeRotation();
    InitalMovingData.CameraRotation = CameraComponent->GetRelativeRotation();
    InitalMovingData.ActorRotation = OwningActor->GetActorRotation();
    InitalMovingData.ActorLocation = OwningActor->GetActorLocation();

    ProgressTimelineFunction.BindUFunction(this, FName("MoveCameraToTargetPosition"));
    TimelineFinishedCallback.BindUFunction(this, FName("FinishCameraMovementToTargetPosistion"));
    TimelineComponent->AddInterpFloat(CurveToUse, ProgressTimelineFunction, FName("Alpha"));
    TimelineComponent->SetTimelineFinishedFunc(TimelineFinishedCallback);
    TimelineComponent->SetLooping(false);
    TimelineComponent->PlayFromStart();
}

void UCCCameraControlComponent::MoveCameraToTargetPosition(float Value)
{
    const FVector NewActorLocation = FMath::Lerp(InitalMovingData.ActorLocation, TargetMovingData.ActorLocation, Value);
    const float NewArmLength = FMath::Lerp(InitalMovingData.SpringArmLength, TargetMovingData.SpringArmLength, Value);
    const FRotator NewCameraRotation = FMath::Lerp(InitalMovingData.CameraRotation, TargetMovingData.CameraRotation, Value);
    const FRotator NewArmRotation = FMath::Lerp(InitalMovingData.SpringArmRotation, TargetMovingData.SpringArmRotation, Value);
    const FRotator NewActorRotation = FMath::Lerp(InitalMovingData.ActorRotation, TargetMovingData.ActorRotation, Value);

    SpringArmComponent->TargetArmLength = NewArmLength;
    SpringArmComponent->SetRelativeRotation(NewArmRotation);
    CameraComponent->SetRelativeRotation(NewCameraRotation);
    OwningActor->SetActorRotation(NewActorRotation);
    OwningActor->SetActorLocation(NewActorLocation);
}

void UCCCameraControlComponent::FinishCameraMovementToTargetPosistion()
{
    bIsCameraMoving = false;

    if (bIsCameraFolowPawn)
    {
        OwningActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
        bIsCameraFolowPawn = false;
    }

    if (bShouldActorBeAttach && OwningActor->GetSelectedPawnSceneComponent())
    {
        OwningActor->AttachToComponent(OwningActor->GetSelectedPawnSceneComponent(), FAttachmentTransformRules::KeepWorldTransform);
        bIsCameraFolowPawn = true;
    }
}