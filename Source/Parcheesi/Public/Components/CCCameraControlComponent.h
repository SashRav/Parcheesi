// Copyright Cats and Cubes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "Components/TimelineComponent.h"
#include "CCCoreTypes.h"
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

    UFUNCTION()
    void MoveCameraToTargetPosition(float Value);

    UFUNCTION()
    void FinishCameraMovementToTargetPosistion();

    void ZoomCamera(const FInputActionValue& Value);
    void RotateCamera(const FInputActionInstance& Value);
    void MoveCameraOnLevel(const FInputActionValue& Value);
    void SetCameraInitPosition(const FName Tag);
    void ResetCameraToDefault();
    void TryToResetCameraAfterPawnMove();
    void MoveCameraToPawn(FVector TargetLocation);
    void ResetCameraLag();

    UPROPERTY(EditAnywhere)
    UCurveFloat* CameraMovementToDefaultCurve;

    UPROPERTY(EditAnywhere)
    UCurveFloat* CameraMovementToPawnCurve;

    UPROPERTY(EditAnywhere)
    UCurveFloat* DefaultCameraMovementCurve;

    UPROPERTY(EditAnywhere)
    UCurveFloat* EndCameraMovementCurve;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UCameraComponent* CameraComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    USpringArmComponent* SpringArmComponent;

    UPROPERTY()
    UTimelineComponent* TimelineComponent;

protected:
    virtual void BeginPlay() override;

    void ZoomCameraFromPawn(float ZoomCameraValue);
    void StartMoveCameraToTargetPositon(UCurveFloat* CurveToUse, FCameraMovemntData TargetData);

    bool bIsCameraMoving = false;
    bool bIsCameraFolowPawn = false;

    FOnTimelineFloat ProgressTimelineFunction;
    FOnTimelineEvent TimelineFinishedCallback;

    FCameraMovemntData InitalMovingData;
    FCameraMovemntData TargetMovingData;
    FCameraMovemntData DefaultMovingData;
    FCameraMovemntData PawnMovingData;

    bool bShouldActorBeAttach = false;
    bool bIsCameraInDefaultState = true;

    const float DefaultRotationLagSpeed = 10.0f;

    UPROPERTY()
    ACCPlayerPawnGame* OwningActor;
};
