// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InputActionValue.h"
#include "Components/TimelineComponent.h"
#include "CCCameraControlComponent.generated.h"

class USpringArmComponent;
class UTimelineComponent;
class UCameraComponent;
class ACCPlayerPawnGame;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PARCHEESI_API UCCCameraControlComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCCCameraControlComponent();

    void ZoomCamera(const FInputActionValue& Value);
    void RotateCamera(const FInputActionValue& Value);
    void SetCameraInitPosition(const FName Tag);
    void ResetCameraToDefault();
    void MoveActorToNewPosition(FVector NewPositionFromMove, FVector NewPositionToMove);

    UFUNCTION()
    void MoveCameraToDefaultPosition(float Value);

    UFUNCTION()
    void FinishCameraMovementToDefaultPosistion();

    UFUNCTION()
    void MoveActorToSelectedPosition(float Value);

    UFUNCTION()
    void FinishActorMovementToSelectedPosistion();

    UPROPERTY(EditAnywhere)
    UCurveFloat* CameraMovementToDefaultCurve;

    UPROPERTY(EditAnywhere)
    UCurveFloat* CameraMovementToPawnCurve;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UCameraComponent* CameraComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    USpringArmComponent* SpringArmComponent;

    UPROPERTY()
    UTimelineComponent* TimelineComponent;

protected:
    virtual void BeginPlay() override;

    void ZoomCameraFromPawn(float ZoomCameraValue);

    bool bIsCameraMoving = false;
    bool bIsCameraFolowPawn = false;
    bool bIsCameraInDefaultState = true;
    float DefaultSpringArmLenght = 5700.0f;
    float PawnSelectedSpringArmLenght = 3000.0f;
    float InitialArmLength;
    float PawnSpringArmRotationPitch = -60.0f;
    FRotator InitialArmRotation;
    FRotator InitialActorRotation;
    FRotator InitalCameraRotation;
    FRotator DefaultActorRotation = FRotator(0.0f, 0.0f, 0.0f);
    FRotator DefaultSpringArmRotation = FRotator(-70.0f, 0.0f, 0.0f);
    FRotator PawnSpringArmRotationTarget;
    FRotator PawnCameraRotation = FRotator(20.0f, 0.0f, 0.0f);
    FVector DefaultActorLocation;
    FVector PositionFromMove;
    FVector PositionToMove;
    FOnTimelineFloat ProgressTimelineFunction;
    FOnTimelineEvent TimelineFinishedCallback;

    UPROPERTY()
    ACCPlayerPawnGame* OwningActor;
};
