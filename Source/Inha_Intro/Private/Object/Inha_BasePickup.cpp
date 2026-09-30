// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Inha_BasePickup.h"

#include "Components/SphereComponent.h"
#include "Player/BaseCharacter.h"

// Sets default values
AInha_BasePickup::AInha_BasePickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SphereCollision = CreateDefaultSubobject<USphereComponent>("SphereCollision");
	SetRootComponent(SphereCollision);
	SphereCollision->SetGenerateOverlapEvents(true);
	SphereCollision->SetSphereRadius(200.f);
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(SphereCollision);
	Mesh->SetCollisionEnabled( ECollisionEnabled::NoCollision);
	
	bReplicates = true;
}

// Called when the game starts or when spawned
void AInha_BasePickup::BeginPlay()
{
	Super::BeginPlay();
	
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnBeginOverlap);
}

void AInha_BasePickup::Pickup_Implementation(ABaseCharacter* OwningCharacter)
{
	SetOwner(OwningCharacter);
}

void AInha_BasePickup::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (const auto CastedCharacter = Cast<ABaseCharacter>(OtherActor))
	{
		Pickup(CastedCharacter);
	}
}

// Called every frame
void AInha_BasePickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

