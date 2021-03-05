// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "TPSCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Materials/Material.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"

ATPSCharacter::ATPSCharacter()
{
    // Set size for player capsule
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

    // Don't rotate character to camera direction
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // Configure character movement
    GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
    GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
    GetCharacterMovement()->bConstrainToPlane = true;
    GetCharacterMovement()->bSnapToPlaneAtStart = true;

    // Create a camera boom...
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->bAbsoluteRotation = true; // Don't want arm to rotate when character does
    CameraBoom->TargetArmLength = 800.f;
    CameraBoom->RelativeRotation = FRotator(-60.f, 0.f, 0.f);
    CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

    // Create a camera...
    TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
    TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

    // Create a decal in the world to show the cursor's location
    //CursorToWorld = CreateDefaultSubobject<UDecalComponent>("CursorToWorld");
    //CursorToWorld->SetupAttachment(RootComponent);
    //static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(TEXT("Material'/Game/Blueprint/Character/M_Cursor_Decal.M_Cursor_Decal'"));
    //if (DecalMaterialAsset.Succeeded())
    //{
    //	CursorToWorld->SetDecalMaterial(DecalMaterialAsset.Object);
    //}
    //CursorToWorld->DecalSize = FVector(16.0f, 32.0f, 32.0f);
    //CursorToWorld->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());

    // Activate ticking in order to update the cursor every frame.
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;
}

void ATPSCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    //if (CursorToWorld != nullptr) //Remove Epic Code
    //{
    //	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
    //	{
    //		if (UWorld* World = GetWorld())
    //		{
    //			FHitResult HitResult;
    //			FCollisionQueryParams Params(NAME_None, FCollisionQueryParams::GetUnknownStatId());
    //			FVector StartLocation = TopDownCameraComponent->GetComponentLocation();
    //			FVector EndLocation = TopDownCameraComponent->GetComponentRotation().Vector() * 2000.0f;
    //			Params.AddIgnoredActor(this);
    //			World->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, Params);
    //			FQuat SurfaceRotation = HitResult.ImpactNormal.ToOrientationRotator().Quaternion();
    //			CursorToWorld->SetWorldLocationAndRotation(HitResult.Location, SurfaceRotation);
    //		}
    //	}
    //	else if ((APlayerController* PC = Cast<APlayerController>(GetController()))
    //	{
    //		FHitResult TraceHitResult;
    //		PC->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);
    //		FVector CursorFV = TraceHitResult.ImpactNormal;
    //		FRotator CursorR = CursorFV.Rotation();
    //		CursorToWorld->SetWorldLocation(TraceHitResult.Location);
    //		CursorToWorld->SetWorldRotation(CursorR);
    //	}
    //}

    if (CurrentCursor)
    {
        APlayerController *myPC = Cast<APlayerController>(GetController());
        if (myPC)
        {
            FHitResult TraceHitResult;
            myPC->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);
            FVector CursorFV = TraceHitResult.ImpactNormal;
            FRotator CursorR = CursorFV.Rotation();

            CurrentCursor->SetWorldLocation(TraceHitResult.Location);
            CurrentCursor->SetWorldRotation(CursorR);
        }
    }

    MovementTick(DeltaSeconds);
}

void ATPSCharacter::BeginPlay()
{
    Super::BeginPlay();

    InitWeapon();

    if (CursorMaterial)
    {
        CurrentCursor = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), CursorMaterial, CursorSize, FVector(0));
    }
}

void ATPSCharacter::SetupPlayerInputComponent(UInputComponent *NewInputComponent)
{
    Super::SetupPlayerInputComponent(NewInputComponent);

    NewInputComponent->BindAxis(TEXT("MoveForward"), this, &ATPSCharacter::InputAxisX);
    NewInputComponent->BindAxis(TEXT("MoveRight"), this, &ATPSCharacter::InputAxisY);

    NewInputComponent->BindAction(TEXT("FireEvent"), EInputEvent::IE_Pressed, this, &ATPSCharacter::InputAttackPressed);
    NewInputComponent->BindAction(TEXT("FireEvent"), EInputEvent::IE_Released, this, &ATPSCharacter::InputAttackReleased);
}

void ATPSCharacter::InputAxisY(float Value)
{
    AxisY = Value;
}

void ATPSCharacter::InputAxisX(float Value)
{
    AxisX = Value;
}

void ATPSCharacter::InputAttackPressed()
{
    AttackCharEvent(true);
}

void ATPSCharacter::InputAttackReleased()
{
    AttackCharEvent(false);
}

void ATPSCharacter::MovementTick(float DeltaTime)
{
    AddMovementInput(FVector(1.0f, 0.0f, 0.0f), AxisX);
    AddMovementInput(FVector(0.0f, 1.0f, 0.0f), AxisY);

    if (MovementState == EMovementState::SprintRun_State)
    {
        FVector myRotationVector = FVector(AxisX, AxisY, 0.0f);
        FRotator myRotator = myRotationVector.ToOrientationRotator();
        SetActorRotation((FQuat(myRotator)));
    }
    else
    {
        APlayerController *myController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
        if (myController)
        {
            FHitResult ResultHit;
            //myController->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery6, false, ResultHit);// bug was here Config\DefaultEngine.Ini
            myController->GetHitResultUnderCursor(ECC_GameTraceChannel1, true, ResultHit);

            float FindRotaterResultYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ResultHit.Location).Yaw;
            SetActorRotation(FQuat(FRotator(0.0f, FindRotaterResultYaw, 0.0f)));
        }
    }
}

void ATPSCharacter::AttackCharEvent(bool bIsFiring)
{
    AWeaponDefault *myWeapon = nullptr;
    myWeapon = GetCurrentWeapon();
    if (myWeapon)
    {
        //ToDo Check melee or range
        myWeapon->SetWeaponStateFire(bIsFiring);
    }
    else
        UE_LOG(LogTemp, Warning, TEXT("ATPSCharacter::AttackCharEvent - CurrentWeapon -NULL"));
}

void ATPSCharacter::CharacterUpdate()
{
    float ResSpeed = 600.0f;
    switch (MovementState)
    {
    case EMovementState::Aim_State:
        ResSpeed = MovementSpeedInfo.AimSpeedNormal;
        break;
    case EMovementState::AimWalk_State:
        ResSpeed = MovementSpeedInfo.AimSpeedWalk;
        break;
    case EMovementState::Walk_State:
        ResSpeed = MovementSpeedInfo.WalkSpeedNormal;
        break;
    case EMovementState::Run_State:
        ResSpeed = MovementSpeedInfo.RunSpeedNormal;
        break;
    case EMovementState::SprintRun_State:
        ResSpeed = MovementSpeedInfo.SprintRunSpeedRun;
        break;
    default:
        break;
    }

    GetCharacterMovement()->MaxWalkSpeed = ResSpeed;
}

void ATPSCharacter::ChangeMovementState()
{
    if (!WalkEnabled && !SprintRunEnabled && !AimEnabled)
    {
        MovementState = EMovementState::Run_State;
    }
    else
    {
        if (SprintRunEnabled)
        {
            WalkEnabled = false;
            AimEnabled = false;
            MovementState = EMovementState::SprintRun_State;
        }
        if (WalkEnabled && !SprintRunEnabled && AimEnabled)
        {
            MovementState = EMovementState::AimWalk_State;
        }
        else
        {
            if (WalkEnabled && !SprintRunEnabled && !AimEnabled)
            {
                MovementState = EMovementState::Walk_State;
            }
            else
            {
                if (!WalkEnabled && !SprintRunEnabled && AimEnabled)
                {
                    MovementState = EMovementState::Aim_State;
                }
            }
        }
    }
    CharacterUpdate();

    //Weapon state update
    AWeaponDefault *myWeapon = GetCurrentWeapon();
    if (myWeapon)
    {
        myWeapon->UpdateStateWeapon(MovementState);
    }
}

AWeaponDefault *ATPSCharacter::GetCurrentWeapon()
{
    return CurrentWeapon;
}

void ATPSCharacter::InitWeapon() //ToDo Init by id row by table
{
    if (InitWeaponClass)
    {
        FVector SpawnLocation = FVector(0);
        FRotator SpawnRotation = FRotator(0);

        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        SpawnParams.Owner = GetOwner();
        SpawnParams.Instigator = GetInstigator();

        AWeaponDefault *myWeapon = Cast<AWeaponDefault>(GetWorld()->SpawnActor(InitWeaponClass, &SpawnLocation, &SpawnRotation, SpawnParams));
        if (myWeapon)
        {
            FAttachmentTransformRules Rule(EAttachmentRule::SnapToTarget, false);
            myWeapon->AttachToComponent(GetMesh(), Rule, FName("WeaponSocketRightHand"));
            CurrentWeapon = myWeapon;

            myWeapon->UpdateStateWeapon(MovementState);
        }
    }
}

UDecalComponent *ATPSCharacter::GetCursorToWorld()
{
    return CurrentCursor;
}