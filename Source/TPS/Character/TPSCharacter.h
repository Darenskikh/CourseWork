// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FuncLibrary/Types.h"
#include "TPSCharacter.generated.h"

UCLASS(Blueprintable)
class ATPSCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ATPSCharacter();

    // Called every frame.
    virtual void Tick(float DeltaSeconds) override;

    virtual void SetupPlayerInputComponent(class UInputComponent *InputComponent) override;

    /** Returns TopDownCameraComponent subobject **/
    FORCEINLINE class UCameraComponent *GetTopDownCameraComponent() const { return TopDownCameraComponent; }
    /** Returns CameraBoom subobject **/
    FORCEINLINE class USpringArmComponent *GetCameraBoom() const { return CameraBoom; }
    /** Returns CursorToWorld subobject **/
    FORCEINLINE class UDecalComponent *GetCursorToWorld() { return CursorToWorld; }

private:
    /** Top down camera */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class UCameraComponent *TopDownCameraComponent;

    /** Camera boom positioning the camera above the character */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class USpringArmComponent *CameraBoom;

    /** A decal that projects to the cursor location. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class UDecalComponent *CursorToWorld;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    EMovementState MovementState = EMovementState::Run_State;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    FCharacterSpeed MovementInfo;

    UFUNCTION()
    void InputAxisY(float Value);
    UFUNCTION()
    void InputAxisX(float Value);

    float AxisX = 0.0f;
    float AxisY = 0.0f;
    // Tick Func
    UFUNCTION()
    void MovementTick(float DeltaTime);

    UFUNCTION(BlueprintCallable)
    void CharacterUpdate();
    UFUNCTION(BlueprintCallable)
    void ChangeMovementState(EMovementState NewMovementState);
};