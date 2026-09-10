// Fill out your copyright notice in the Description page of Project Settings.

#include "Public/Player/BaseCharacter.h"

#include "Camera/CameraComponent.h"
#include "Component/PlayerAttackComponent.h"
#include "Component/PlayerMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SocketOffset = FVector(0, 70, 90);
	CameraBoom->TargetArmLength = 400.f;
	CameraBoom->ProbeSize = 20.f;
	
	FollowCamera = CreateDefaultSubobject<UCameraComponent>("FollowCamera");
	FollowCamera->SetupAttachment(CameraBoom);
	
	PlayerMovementComponent = CreateDefaultSubobject<UPlayerMovementComponent>("PlayerMovementComponent");
	if (UPlayerMovementComponent* CastedMoventComponent = Cast<UPlayerMovementComponent>(PlayerMovementComponent))
	{
		GetCharacterMovement()->MaxWalkSpeed = CastedMoventComponent->GetWalkSpeed();
	}
	
	PlayerAttackComponent = CreateDefaultSubobject<UPlayerAttackComponent>("PlayerAttackComponent");
	if (GetCastedAttackComponent())
	{
		SetCameraFOV(GetCastedAttackComponent()->GetBaseFOV());
	}
	ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshPath(TEXT("/Script/Engine.SkeletalMesh'/Game/Assets/ThirdPersonTemplate/Characters/Mannequins/Meshes/SKM_Quinn_Simple.SKM_Quinn_Simple'"));
	
	if (MeshPath.Succeeded())
	{
		USkeletalMeshComponent* SkeletalMeshComponent = GetMesh();
		SkeletalMeshComponent->SetSkeletalMesh(MeshPath.Object);
		SkeletalMeshComponent->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
	}
	
	RifleMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Rifle");
	RifleMesh->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));
	ConstructorHelpers::FObjectFinder<USkeletalMesh> RifleMeshPath(TEXT("/Script/Engine.SkeletalMesh'/Game/Assets/MilitaryWeapSilver/Weapons/Assault_Rifle_A.Assault_Rifle_A'"));
	if (RifleMeshPath.Succeeded())
	{
		RifleMesh->SetSkeletalMesh(RifleMeshPath.Object);
		RifleMesh->SetVisibility(true);
	}
	
	SniperMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Sniper");
	SniperMesh->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));	
	ConstructorHelpers::FObjectFinder<USkeletalMesh> SniperMeshPath(TEXT("/Script/Engine.SkeletalMesh'/Game/Assets/MilitaryWeapSilver/Weapons/Sniper_Rifle_A.Sniper_Rifle_A'"));
	if (SniperMeshPath.Succeeded())
	{
		SniperMesh->SetSkeletalMesh(SniperMeshPath.Object);
		SniperMesh->SetVisibility(false);
	}
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetCastedAttackComponent())
	{
		GetCastedAttackComponent()->OnRifleFire.BindUObject(this, &ThisClass::OnFireCallback);
	}
}

void ABaseCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	Hp = InitHp;
}

APlayerController* ABaseCharacter::GetPlayerController()
{
	if (!CachedPlayerController)
	{
		CachedPlayerController = Cast<APlayerController>(GetController());
	}
	return CachedPlayerController;
}

UPlayerAttackComponent* ABaseCharacter::GetCastedAttackComponent()
{
	if (!CastedAttackComponent)
	{
		CastedAttackComponent = Cast<UPlayerAttackComponent>(PlayerAttackComponent);
	}
	return CastedAttackComponent;
}

void ABaseCharacter::OnHitEvent()
{
	--Hp;
	if (Hp <= 0)
	{
		GameOver();
	}
}

void ABaseCharacter::GameOver()
{
	OnGameOver.Broadcast();
	UGameplayStatics::SetGamePaused(GetWorld(), true);
}

void ABaseCharacter::SetSkeletalMeshVisibility(USkeletalMeshComponent* Target, bool Visible)
{
	if (Target)
	{
		Target->SetVisibility(Visible);
	}
}

void ABaseCharacter::OnFireCallback()
{
	if (FireRifleMontage)
	{
		PlayAnimMontage(FireRifleMontage);
	}
	OnFire.ExecuteIfBound();
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

void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
}

void ABaseCharacter::Fire()
{
	if (!GetCastedAttackComponent()) return;
	GetCastedAttackComponent()->Fire();
}

 void ABaseCharacter::ToggleWeapon()
 {
	if (!GetCastedAttackComponent()) return;
	RifleMesh->ToggleVisibility();
	SniperMesh->ToggleVisibility();
	GetCastedAttackComponent()->ToggleWeapon();
	OnWeaponChange.Broadcast();
 }

 bool ABaseCharacter::EnterSniper()
 {
	if (!GetCastedAttackComponent()) return false;
	return GetCastedAttackComponent()->EnterSniper();
 }
//
 bool ABaseCharacter::ExitSniper()
 {
	if (!GetCastedAttackComponent()) return false;
	return GetCastedAttackComponent()->ExitSniper();
 }

void ABaseCharacter::SetCameraFOV(float NewFOV) const
{
	if (!FollowCamera) return;
	FollowCamera->SetFieldOfView(NewFOV);
}

FTransform ABaseCharacter::GetFireTransform() const
{
	return RifleMesh->GetSocketTransform(TEXT("MuzzleFlash"));
}

FVector ABaseCharacter::GetCameraLocation() const
{
	if (!FollowCamera) return FVector();
	return FollowCamera->GetComponentLocation();
}

FVector ABaseCharacter::GetCameraForwardVector() const
{
	if (!FollowCamera) return FVector();
	return FollowCamera->GetForwardVector();
}

UPlayerBaseComponent* ABaseCharacter::GetPlayerMovementComponent() const
{
	return PlayerMovementComponent;
}

UPlayerBaseComponent* ABaseCharacter::GetPlayerAttackComponent() const
{
	return PlayerAttackComponent;
}
