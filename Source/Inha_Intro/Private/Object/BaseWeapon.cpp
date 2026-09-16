// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/BaseWeapon.h"

#include "Components/SphereComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Player/BaseCharacter.h"

// Sets default values
ABaseWeapon::ABaseWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));

	// 시작 무기는 가지고 태어날거기때문에 충돌체를 끄고 시작
	//CollisionComp->SetCollisionProfileName(TEXT("NoCollision"));
	CollisionComp->SetSphereRadius(64);

	// 충돌을 루트로 설정
	RootComponent = CollisionComp;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(CollisionComp);
}

bool ABaseWeapon::EnterSniper()
{
	return false;
}

bool ABaseWeapon::ExitSniper()
{
	return false;
}

// Called when the game starts or when spawned
void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();

	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnOverlap);
}

void ABaseWeapon::Fire()
{
}

USkeletalMeshComponent* ABaseWeapon::GetMesh()
{
	return WeaponMesh;
}

EWeaponType ABaseWeapon::GetWeaponType()
{
	return WeaponType;
}

ABaseCharacter* ABaseWeapon::GetCastedCharacter()
{
	if (!CastedOwnerCharacter)
	{
		CastedOwnerCharacter = Cast<ABaseCharacter>(GetOwner());
	}
	return CastedOwnerCharacter;
}

void ABaseWeapon::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ABaseCharacter* CastedActor = Cast<ABaseCharacter>(OtherActor))
	{
		SetOwner(CastedActor);
		CastedOwnerCharacter = CastedActor;
		const USkeletalMeshSocket* HandSocket = CastedActor->GetMesh()->GetSocketByName(FName("hand_rSocket"));
		if (HandSocket)
		{
			HandSocket->AttachActor(this, CastedActor->GetMesh());
			CollisionComp->SetCollisionProfileName(TEXT("NoCollision"));
			CollisionComp->SetGenerateOverlapEvents(false);
			if (CastedActor->GetWeaponNum() != 0)
			{
				SetActorHiddenInGame(true);
			}
			CastedActor->PickupWeapon(this);
		}
	}
}
