// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectileDefault.h"

// Sets default values
AProjectileDefault::AProjectileDefault()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    BulletCollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Sphere"));

    BulletCollisionSphere->SetSphereRadius(16.f);

    BulletCollisionSphere->OnComponentHit.AddDynamic(this, &AProjectileDefault::BulletCollisionSphereHit);
    BulletCollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AProjectileDefault::BulletCollisionSphereBeginOverlap);
    BulletCollisionSphere->OnComponentEndOverlap.AddDynamic(this, &AProjectileDefault::BulletCollisionSphereEndOverlap);

    BulletCollisionSphere->bReturnMaterialOnMove = true; //hit event return physMaterial

    BulletCollisionSphere->SetCanEverAffectNavigation(false); //collision not affect navigation (P keybord on editor)

    RootComponent = BulletCollisionSphere;

    BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bullet Projectile Mesh"));
    BulletMesh->SetupAttachment(RootComponent);
    BulletMesh->SetCanEverAffectNavigation(false);

    BulletFX = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Bullet FX"));
    BulletFX->SetupAttachment(RootComponent);

    //BulletSound = CreateDefaultSubobject<UAudioComponent>(TEXT("Bullet Audio"));
    //BulletSound->SetupAttachment(RootComponent);

    BulletProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Bullet ProjectileMovement"));
    BulletProjectileMovement->UpdatedComponent = RootComponent;
    BulletProjectileMovement->InitialSpeed = 1.f;
    BulletProjectileMovement->MaxSpeed = 0.f;

    BulletProjectileMovement->bRotationFollowsVelocity = true;
    BulletProjectileMovement->bShouldBounce = true;
}

// Called when the game starts or when spawned
void AProjectileDefault::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void AProjectileDefault::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AProjectileDefault::BulletCollisionSphereHit(UPrimitiveComponent *HitComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, FVector NormalImpulse, const FHitResult &Hit)
{
}

void AProjectileDefault::BulletCollisionSphereBeginOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
}

void AProjectileDefault::BulletCollisionSphereEndOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex)
{
}
