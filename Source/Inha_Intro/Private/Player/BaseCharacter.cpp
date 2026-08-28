// Fill out your copyright notice in the Description page of Project Settings.

#include "Public/Player/BaseCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshPath(TEXT("/Script/Engine.SkeletalMesh'/Game/Assets/ThirdPersonTemplate/Characters/Mannequins/Meshes/SKM_Quinn_Simple.SKM_Quinn_Simple'"));
	
	if (MeshPath.Succeeded())
	{
		USkeletalMeshComponent* SkeletalMeshComponent = GetMesh();
		SkeletalMeshComponent->SetSkeletalMesh(MeshPath.Object);
		SkeletalMeshComponent->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
	}
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(RootComponent);
	//CameraBoom->SetRelativeLocation(FVector(0, 70, 90));
	CameraBoom->SocketOffset = FVector(0, 70, 90);
	CameraBoom->TargetArmLength = 400.f;
	CameraBoom->ProbeSize = 20.f;
	
	FollowCamera = CreateDefaultSubobject<UCameraComponent>("FollowCamera");
	FollowCamera->SetupAttachment(CameraBoom);
	FollowCamera->SetFieldOfView(BaseFov);
	
	RifleMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Rilfe");
	RifleMesh->SetupAttachment(GetMesh());
	ConstructorHelpers::FObjectFinder<USkeletalMesh> RifleMeshPath(TEXT("/Script/Engine.SkeletalMesh'/Game/Assets/MilitaryWeapSilver/Weapons/Assault_Rifle_A.Assault_Rifle_A'"));
	if (RifleMeshPath.Succeeded())
	{
		RifleMesh->SetSkeletalMesh(RifleMeshPath.Object);
		RifleMesh->SetRelativeLocation(FVector(-14, 11, 138));
		if (EquipWeapon != EEquipWeapon::Rifle)
		{
			RifleMesh->SetVisibility(false);
		}
	}
	
	SniperMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Sniper");
	SniperMesh->SetupAttachment(GetMesh());
	ConstructorHelpers::FObjectFinder<USkeletalMesh> SniperMeshPath(TEXT("/Script/Engine.SkeletalMesh'/Game/Assets/MilitaryWeapSilver/Weapons/Sniper_Rifle_A.Sniper_Rifle_A'"));
	if (SniperMeshPath.Succeeded())
	{
		SniperMesh->SetSkeletalMesh(SniperMeshPath.Object);
		SniperMesh->SetRelativeLocation(FVector(-22, 31, 128));
		if (EquipWeapon != EEquipWeapon::Sniper)
		{
			SniperMesh->SetVisibility(false);
		}
	}
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::SetSkeletalMeshVisibility(USkeletalMeshComponent* Target, bool Visible)
{
	if (Target)
	{
		Target->SetVisibility(Visible);
	}
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	float dist = FVector::Dist(GetActorLocation(), FollowCamera->GetComponentLocation());
	if (dist <= HiddenMeshDist)
	{
		GetMesh()->SetVisibility(false);
	}
	else
	{
		GetMesh()->SetVisibility(true);
	}
}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABaseCharacter::ToggleWeapon()
{
	switch(EquipWeapon)
	{
	case EEquipWeapon::Rifle:
		SetSkeletalMeshVisibility(RifleMesh, false);
		SetSkeletalMeshVisibility(SniperMesh, true);
		EquipWeapon = EEquipWeapon::Sniper;
		break;
	case EEquipWeapon::Sniper:
	case EEquipWeapon::None:
	default:
		SetSkeletalMeshVisibility(RifleMesh, true);
		SetSkeletalMeshVisibility(SniperMesh, false);
		EquipWeapon = EEquipWeapon::Rifle;
		break;
	}
}

bool ABaseCharacter::EnterSniper()
{
	if (EquipWeapon != EEquipWeapon::Sniper) return false;
	FollowCamera->SetFieldOfView(InSniperFov);
	
	return true;
}

bool ABaseCharacter::ExitSniper()
{
	if (EquipWeapon != EEquipWeapon::Sniper) return false;
	FollowCamera->SetFieldOfView(BaseFov);
	
	return true;
}

FTransform ABaseCharacter::GetFirePosition() const
{
	return RifleMesh->GetSocketTransform(TEXT("MuzzleFlash"));
}

EEquipWeapon ABaseCharacter::GetEquipWeapon() const
{
	return EquipWeapon;
}
