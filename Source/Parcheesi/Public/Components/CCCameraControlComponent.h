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
class UCineCameraComponent;
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
    UCineCameraComponent* CineCameraComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    USpringArmComponent* SpringArmComponent;

    UPROPERTY()
    UTimelineComponent* TimelineComponent;

protected:
    virtual void BeginPlay() override;

    void StartMoveCameraToTargetPositon(UCurveFloat* CurveToUse, FCameraMovemntData TargetData);

    bool bIsCameraMoving = false;
    bool bIsCameraFolowPawn = false;
    bool bIsCameraMovingToPoint = false;

    FOnTimelineFloat ProgressTimelineFunction;
    FOnTimelineEvent TimelineFinishedCallback;

    FCameraMovemntData InitalMovingData;
    FCameraMovemntData TargetMovingData;
    FCameraMovemntData DefaultMovingData;
    FCameraMovemntData PawnMovingData;

    bool bShouldActorBeAttach = false;
    bool bIsCameraInDefaultState = true;
    bool bShouldMoveOnlyFocalLength = false;
    bool bIsPawnChangedWhenFocused = false;

    FVector TargetSocketOffset;
    FVector InitalSocketOffset;

    float StartFocalLength;

    UPROPERTY(EditDefaultsOnly, Category = "Camera Lag")
    float DefaultRotationLagSpeed = 10.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Camera Lag")
    float DefaultLagSpeed = 6.5f;

    UPROPERTY(EditDefaultsOnly, Category = "Zoom | Default")
    FVector DefaultSocketOffset = FVector(0.0f, 0.0f, -300.0f);

    UPROPERTY(EditDefaultsOnly, Category = "Zoom | Default")
    float MinFocalLength = 12.5f;

    UPROPERTY(EditDefaultsOnly, Category = "Zoom | Default")
    float MaxFocalLength = 30.0f;

    UPROPERTY(EditDefaultsOnly)
    float MaxSpringArmLength = 3200.f;

    UPROPERTY(EditDefaultsOnly, Category = "Zoom | Default")
    FRotator DefaultSpringArmRotation = FRotator(-70.0f, 0.0f, 0.0f);

    UPROPERTY(EditDefaultsOnly, Category = "Zoom | Default")
    FRotator DefaultCameraRotation = FRotator(0.0f, 0.0f, 0.0f);

    UPROPERTY(EditDefaultsOnly, Category = "Zoom | Pawn")
    FVector PawnSocketOffset = FVector(0.0f, 0.0f, 1000.0f);

    UPROPERTY(EditDefaultsOnly, Category = "Zoom | Pawn")
    FRotator DefaultPawnSpringArmRotation = FRotator(-60.0f, 0.0f, 0.0f);

    UPROPERTY(EditDefaultsOnly, Category = "Zoom | Pawn")
    FRotator DefaultPawnCameraRotation = FRotator(-20.0f, 0.0f, 0.0f);

    UPROPERTY(EditDefaultsOnly, Category = "Zoom | Pawn")
    float MinPawnFocalLength = 20.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Zoom | Pawn")
    float MaxPawnFocalLength = 35.0f;

    UPROPERTY()
    ACCPlayerPawnGame* OwningActor;
};
