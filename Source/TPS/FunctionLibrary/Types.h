// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Types.generated.h"

UENUM(BlueprintType)
enum class EMovementState : uint8
{
    Aim_State UMETA(DisplayName = "Aim State"),
    Walk_State UMETA(DisplayName = "Walk State"),
    Run_State UMETA(DisplayName = "Run State"),
};

USTRUCT(BlueprintType)
struct FCharacterSpeed
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float AimSpeed = 300.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float WalkSpeed = 200.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float RunSpeed = 600.0f;
};

UCLASS()
class TPS_API UTypes : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
};