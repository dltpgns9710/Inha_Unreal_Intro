// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile/BaseBullet.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ABaseBullet::ABaseBullet()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	SphereComponent->SetCollisionProfileName("BlockAll");
	SphereComponent->SetSphereRadius(13);
	SetRootComponent(SphereComponent);
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	StaticMeshComponent->SetupAttachment(RootComponent);
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticMeshComponent->SetRelativeScale3D(FVector(4.f));
	StaticMeshComponent->SetRelativeRotation(FRotator(-90,0,0));
	
	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("MovementComponent");
	MovementComponent->SetUpdatedComponent(SphereComponent);
	
	MovementComponent->InitialSpeed = 5000.f;
	MovementComponent->MaxSpeed = 5000.f;
	MovementComponent->bShouldBounce = true;
	MovementComponent->Bounciness = .3f;
	
	InitialLifeSpan = LifeSpan;
}

// Called when the game starts or when spawned
void ABaseBullet::BeginPlay()
{
	Super::BeginPlay();
	
	FTimerHandle deathTimerHandle;
	//GetWorld()->GetTimerManager().SetTimer(deathTimerHandle, this, &ABaseBullet::Die, LifeSpan, false);
	GetWorld()->GetTimerManager().SetTimer(deathTimerHandle, 
		FTimerDelegate::CreateLambda([this]()->void
		{
			Destroy();
		}), LifeSpan, false);
}

// Called every frame
void ABaseBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseBullet::Die()
{
	Destroy();
}

