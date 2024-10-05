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

    
    SpringArmComponent->SetRelativeRotation(DefaultSpringArmRotation);
    SpringArmComponent->TargetArmLength = DefaultSpringArmLenght;
    SpringArmComponent->SocketOffset = FVector(0.0f, 0.0f, -700.0f);

    DefaultActorLocation = OwningActor->GetActorLocation();
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

    DefaultActorRotation = NewRotation;
    OwningActor->SetActorRotation(DefaultActorRotation);
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

void UCCCameraControlComponent::ResetCameraToDefault()
{
    if (!SpringArmComponent || !CameraComponent || !CameraMovementToDefaultCurve || bIsCameraInDefaultState)
        return;

    // Create Timeline to move camera back to default position
    // Curve is set from 0 to 1
    OwningActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

    bIsCameraMoving = true;
    InitialArmLength = SpringArmComponent->TargetArmLength;
    InitialArmRotation = SpringArmComponent->GetRelativeRotation();
    InitialActorRotation = OwningActor->GetActorRotation();
    PositionFromMove = OwningActor->GetActorLocation();
    InitalCameraRotation = CameraComponent->GetRelativeRotation();

    ProgressTimelineFunction.BindUFunction(this, FName("MoveCameraToDefaultPosition"));
    TimelineFinishedCallback.BindUFunction(this, FName("FinishCameraMovementToDefaultPosistion"));
    TimelineComponent->AddInterpFloat(CameraMovementToDefaultCurve, ProgressTimelineFunction, FName("Alpha"));
    TimelineComponent->SetTimelineFinishedFunc(TimelineFinishedCallback);
    TimelineComponent->SetLooping(false);
    TimelineComponent->PlayFromStart();
}

void UCCCameraControlComponent::MoveCameraToDefaultPosition(float Value)
{
    const float NewArmLength = FMath::Lerp(InitialArmLength, DefaultSpringArmLenght, Value);
    const FRotator NewActorRotation = FMath::Lerp(InitialActorRotation, DefaultActorRotation, Value);
    const FRotator NewCameraRotation = FMath::Lerp(InitalCameraRotation, FRotator(0.0f, 0.0f, 0.0f), Value);
    const FRotator NewArmRotation = FMath::Lerp(InitialArmRotation, DefaultSpringArmRotation, Value);
    const FVector NewLocation = FMath::Lerp(PositionFromMove, DefaultActorLocation, Value);

    SpringArmComponent->TargetArmLength = NewArmLength;
    SpringArmComponent->SetRelativeRotation(NewArmRotation);
    CameraComponent->SetRelativeRotation(NewCameraRotation);
    OwningActor->SetActorRotation(NewActorRotation);
    OwningActor->SetActorLocation(NewLocation);
}

void UCCCameraControlComponent::FinishCameraMovementToDefaultPosistion()
{
    bIsCameraMoving = false;
    bIsCameraInDefaultState = true;
    bIsCameraFolowPawn = false;
}

void UCCCameraControlComponent::MoveActorToNewPosition(FVector NewPositionFromMove, FVector NewPositionToMove)
{

    if (!SpringArmComponent || !CameraComponent || !CameraMovementToPawnCurve)
        return;

    PositionToMove = NewPositionToMove;
    PositionFromMove = NewPositionFromMove;

    bIsCameraMoving = true;
    bIsCameraInDefaultState = false;
    InitialArmLength = SpringArmComponent->TargetArmLength;
    InitialArmRotation = SpringArmComponent->GetRelativeRotation();
    InitalCameraRotation = CameraComponent->GetRelativeRotation();
    PawnSpringArmRotationTarget = SpringArmComponent->GetRelativeRotation();
    PawnSpringArmRotationTarget.Pitch = PawnSpringArmRotationPitch;

    ProgressTimelineFunction.BindUFunction(this, FName("MoveActorToSelectedPosition"));
    TimelineFinishedCallback.BindUFunction(this, FName("FinishActorMovementToSelectedPosistion"));
    TimelineComponent->AddInterpFloat(CameraMovementToPawnCurve, ProgressTimelineFunction, FName("Alpha"));
    TimelineComponent->SetTimelineFinishedFunc(TimelineFinishedCallback);
    TimelineComponent->SetLooping(false);
    TimelineComponent->PlayFromStart();
}

void UCCCameraControlComponent::MoveActorToSelectedPosition(float Value) 
{
    const FVector NewLocation = FMath::Lerp(PositionFromMove, PositionToMove, Value);
    OwningActor->SetActorLocation(NewLocation);

    if (bIsCameraFolowPawn)
        return;

    const float NewArmLength = FMath::Lerp(InitialArmLength, PawnSelectedSpringArmLenght, Value);
    const FRotator NewCameraRotation = FMath::Lerp(InitalCameraRotation, PawnCameraRotation, Value);
    const FRotator NewArmRotation = FMath::Lerp(InitialArmRotation, PawnSpringArmRotationTarget, Value);

    SpringArmComponent->TargetArmLength = NewArmLength;
    CameraComponent->SetRelativeRotation(NewCameraRotation);
    SpringArmComponent->SetRelativeRotation(NewArmRotation);
}

void UCCCameraControlComponent::FinishActorMovementToSelectedPosistion() 
{
    bIsCameraFolowPawn = true;
    bIsCameraMoving = false;
    PositionToMove = FVector();
    PositionFromMove = FVector();

    OwningActor->AttachToComponent(OwningActor->GetSelectedPawnSceneComponent(), FAttachmentTransformRules::KeepWorldTransform);
}
