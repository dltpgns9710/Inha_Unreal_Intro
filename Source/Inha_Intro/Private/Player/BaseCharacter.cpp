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
		SetCameraFOV(90);
	}
	ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshPath(TEXT("/Script/Engine.SkeletalMesh'/Game/Assets/ThirdPersonTemplate/Characters/Mannequins/Meshes/SKM_Quinn_Simple.SKM_Quinn_Simple'"));
	
	if (MeshPath.Succeeded())
	{
		USkeletalMeshComponent* SkeletalMeshComponent = GetMesh();
		SkeletalMeshComponent->SetSkeletalMesh(MeshPath.Object);
		SkeletalMeshComponent->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
	}
	//
	// RifleMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Rifle");
	// RifleMesh->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));
	// ConstructorHelpers::FObjectFinder<USkeletalMesh> RifleMeshPath(TEXT("/Script/Engine.SkeletalMesh'/Game/Assets/MilitaryWeapSilver/Weapons/Assault_Rifle_A.Assault_Rifle_A'"));
	// if (RifleMeshPath.Succeeded())
	// {
	// 	RifleMesh->SetSkeletalMesh(RifleMeshPath.Object);
	// 	RifleMesh->SetVisibility(true);
	// }
	//
	// SniperMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Sniper");
	// SniperMesh->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));	
	// ConstructorHelpers::FObjectFinder<USkeletalMesh> SniperMeshPath(TEXT("/Script/Engine.SkeletalMesh'/Game/Assets/MilitaryWeapSilver/Weapons/Sniper_Rifle_A.Sniper_Rifle_A'"));
	// if (SniperMeshPath.Succeeded())
	// {
	// 	SniperMesh->SetSkeletalMesh(SniperMeshPath.Object);
	// 	SniperMesh->SetVisibility(false);
	// }
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetCastedAttackComponent())
	{
		GetCastedAttackComponent()->OnRifleFire.BindUObject(this, &ThisClass::OnFireCallback);
	}
	AO_StartYaw = GetController()->GetControlRotation().Yaw;
	
	LinkBaseAnimLayer();
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

void ABaseCharacter::UpdateAimOffset(float DeltaTime)
{
	FVector Velocity = GetVelocity();
	float Speed = Velocity.Size2D();  // 높이에 대한 속도는 무시하고, 수평속도만 계산
	bool bIsInAir = GetCharacterMovement()->IsFalling();

	// 가만히 있을때 AO_Yaw 계산
	if (Speed == 0.f && !bIsInAir)
	{
		// 현재 회전값
		float CurrentYaw = GetController()->GetControlRotation().Yaw;

		// -180~180 사이의 차이값으로 정규화해서 넘겨준다.
		float DeltaYaw = FMath::FindDeltaAngleDegrees(AO_StartYaw, CurrentYaw);

		// 시작 AO YAW 값을 기준으로 차이점을 계산한다.
		AO_Yaw = DeltaYaw;  //CurrentYaw - AO_StartYaw;

		// 에임오프셋이 적용될때는, 컨트롤러가 회전해도 캐릭터는 제자리에 서있어야 한다.
		bUseControllerRotationYaw = false;
	}
	else
	{
		AO_Yaw = 0;
		AO_StartYaw = GetController()->GetControlRotation().Yaw;  // 이동 중에는 시작값을 계속 갱신

		// 달리기 시작하면, TPS 장르처럼 카메라 방향으로 캐릭터가 회전해야한다.
		bUseControllerRotationYaw = true;
	}

	// 액터의 눈의 위치로 Pitch를 계산하면 편하다
	AO_Pitch = GetBaseAimRotation().Pitch;
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
	UpdateAimOffset(DeltaTime);
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
	
	GetCastedAttackComponent()->ToggleWeapon();
	OnWeaponChange.Broadcast();
 }

void ABaseCharacter::LinkBaseAnimLayer()
{
	if (DefaultLayerClass && GetMesh())
	{
		GetMesh()->LinkAnimClassLayers(DefaultLayerClass);
	}
}

void ABaseCharacter::UnlinkBaseAnimLayer()
{
	if (DefaultLayerClass && GetMesh())
	{
		GetMesh()->UnlinkAnimClassLayers(DefaultLayerClass);
	}
}

bool ABaseCharacter::EnterSniper()
{
if (!GetCastedAttackComponent()) return false;
return GetCastedAttackComponent()->EnterSniper();
}

bool ABaseCharacter::ExitSniper()
{
if (!GetCastedAttackComponent()) return false;
return GetCastedAttackComponent()->ExitSniper();\
}

bool ABaseCharacter::IsUnarmed()
{
	if (GetCastedAttackComponent())
	{
		return GetCastedAttackComponent()->WeaponNum() == 0;
	}
	return true;
}

void ABaseCharacter::SetCameraFOV(float NewFOV) const
{
	if (!FollowCamera) return;
	FollowCamera->SetFieldOfView(NewFOV);
}

float ABaseCharacter::GetAoYaw()
{
	return AO_Yaw;
}

float ABaseCharacter::GetAoPitch()
{
	return AO_Pitch;
}

int ABaseCharacter::GetWeaponNum()
{
	if (GetCastedAttackComponent())
	{
		return GetCastedAttackComponent()->WeaponNum();
	}
	return 0;
}

FTransform ABaseCharacter::GetFireTransform()
{
	return GetCastedAttackComponent()->GetWeaponSkeletalMesh()->GetSocketTransform(TEXT("MuzzleFlash"));
}

FVector ABaseCharacter::GetSniperFireLocation()
{
	return GetFireTransform().GetLocation();
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

USkeletalMeshComponent* ABaseCharacter::GetWeaponMesh()
{
	if (GetCastedAttackComponent())
	{
		return GetCastedAttackComponent()->GetWeaponSkeletalMesh();
	}
	return nullptr;
}

void ABaseCharacter::PickupWeapon(ABaseWeapon* TargetWeapon)
{
	GetCastedAttackComponent()->AddWeapon(TargetWeapon);
}
