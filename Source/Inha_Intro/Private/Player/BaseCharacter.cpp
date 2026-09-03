// Fill out your copyright notice in the Description page of Project Settings.

#include "Public/Player/BaseCharacter.h"

#include "NiagaraFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Component/EnemyFSM.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Projectile/BaseBullet.h"

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
	CameraBoom->bUsePawnControlRotation = true;
	//CameraBoom->SetRelativeLocation(FVector(0, 70, 90));
	CameraBoom->SocketOffset = FVector(0, 70, 90);
	CameraBoom->TargetArmLength = 400.f;
	CameraBoom->ProbeSize = 20.f;
	
	FollowCamera = CreateDefaultSubobject<UCameraComponent>("FollowCamera");
	FollowCamera->SetupAttachment(CameraBoom);
	FollowCamera->SetFieldOfView(BaseFov);
	
	RifleMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Rifle");
	RifleMesh->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));
	ConstructorHelpers::FObjectFinder<USkeletalMesh> RifleMeshPath(TEXT("/Script/Engine.SkeletalMesh'/Game/Assets/MilitaryWeapSilver/Weapons/Assault_Rifle_A.Assault_Rifle_A'"));
	if (RifleMeshPath.Succeeded())
	{
		RifleMesh->SetSkeletalMesh(RifleMeshPath.Object);
		//RifleMesh->SetRelativeLocation(FVector(-14, 11, 138));
		if (EquipWeapon != EEquipWeapon::Rifle)
		{
			RifleMesh->SetVisibility(false);
		}
	}
	
	SniperMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Sniper");
	SniperMesh->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));
	ConstructorHelpers::FObjectFinder<USkeletalMesh> SniperMeshPath(TEXT("/Script/Engine.SkeletalMesh'/Game/Assets/MilitaryWeapSilver/Weapons/Sniper_Rifle_A.Sniper_Rifle_A'"));
	if (SniperMeshPath.Succeeded())
	{
		SniperMesh->SetSkeletalMesh(SniperMeshPath.Object);
		//SniperMesh->SetRelativeLocation(FVector(-22, 31, 128));
		if (EquipWeapon != EEquipWeapon::Sniper)
		{
			SniperMesh->SetVisibility(false);
		}
	}
	
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

APlayerController* ABaseCharacter::GetPlayerController()
{
	if (!CachedPlayerController)
	{
		CachedPlayerController = Cast<APlayerController>(GetController());
	}
	return CachedPlayerController;
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

void ABaseCharacter::Fire()
{
	switch(EquipWeapon)
	{
	case EEquipWeapon::Rifle:
		FireRifle();
		break;
	case EEquipWeapon::Sniper:
		FireSniper();
		break;
	case EEquipWeapon::None:
	default:
		break;
	}
}

void ABaseCharacter::FireRifle()
{
	FTransform FirePosition = RifleMesh->GetSocketTransform(TEXT("MuzzleFlash"));
	GetWorld()->SpawnActor<ABaseBullet>(BulletFactory, FirePosition);
	if (FireRifleMontage)
	{
		PlayAnimMontage(FireRifleMontage);
		OnFire.ExecuteIfBound();
	}
	// 카메라 셰이크 재생
	APlayerController* PlayerController = GetPlayerController();
	if (PlayerController && CameraShake)
	{
		PlayerController->PlayerCameraManager->StartCameraShake(CameraShake);
	}
}

void ABaseCharacter::FireSniper()
{
	//FVector StartPosition = SniperMesh->GetSocketLocation(TEXT("MuzzleFlash"));
	FVector StartPosition = FollowCamera->GetComponentLocation();
	FVector EndPosition = StartPosition + FollowCamera->GetForwardVector() * SniperLength;
	//FVector EndPosition = SniperMesh->GetSocketLocation(TEXT("MuzzleFlash")) + FollowCamera->GetForwardVector() * SniperLength;
	FHitResult HitInfo;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitInfo, StartPosition, EndPosition, ECC_GameTraceChannel1, CollisionParams);
	if (bHit)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			EffectFactory,
			HitInfo.ImpactPoint,
			HitInfo.ImpactNormal.Rotation()
		);
		
		UPrimitiveComponent* HitComp = HitInfo.GetComponent();
		if (HitComp && HitComp->IsSimulatingPhysics())
		{
			FVector dir = (EndPosition - StartPosition).GetSafeNormal();
			FVector force = dir * HitComp->GetMass() * 500000;
			HitComp->AddForceAtLocation(force, HitInfo.ImpactPoint);
		}
		
		UObject* Target = HitInfo.GetActor()->GetDefaultSubobjectByName(TEXT("FSM"));
		if (Target)
		{
			UEnemyFSM* EnemyFsm = Cast<UEnemyFSM>(Target);
			EnemyFsm->OnDamageProcess();
		}
	}
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

void ABaseCharacter::SetSpeedToWalk()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void ABaseCharacter::SetSpeedToRun()
{
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

EEquipWeapon ABaseCharacter::GetEquipWeapon() const
{
	return EquipWeapon;
}
