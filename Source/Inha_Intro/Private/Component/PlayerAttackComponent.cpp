// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/PlayerAttackComponent.h"

#include "NiagaraFunctionLibrary.h"
#include "Component/EnemyFSM.h"
#include "Player/BaseCharacter.h"
#include "Projectile/BaseBullet.h"

UPlayerAttackComponent::UPlayerAttackComponent()
{
	
}

void UPlayerAttackComponent::FireRifle()
{
	if (!GetCastedCharacter()) return;
	
	FTransform FirePosition = GetCastedCharacter()->GetFireTransform();
	GetWorld()->SpawnActor<ABaseBullet>(BulletFactory, FirePosition);
	
	OnRifleFire.ExecuteIfBound();
	
	// 카메라 셰이크 재생
	APlayerController* PlayerController = Cast<APlayerController>(GetCastedCharacter()->GetController());
	if (PlayerController && CameraShake)
	{
		PlayerController->PlayerCameraManager->StartCameraShake(CameraShake);
	}
}

void UPlayerAttackComponent::FireSniper()
{
	if (!GetCastedCharacter()) return;
	
	FHitResult HitInfo;
	FCollisionQueryParams CollisionParams;
	FVector StartPosition= GetCastedCharacter()->GetCameraLocation();
	FVector EndPosition= StartPosition + GetCastedCharacter()->GetCameraForwardVector() * SniperLength;
	
	CollisionParams.AddIgnoredActor(GetCastedCharacter());
	
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

void UPlayerAttackComponent::SetSkeletalMeshVisibility(USkeletalMeshComponent* Target, bool Visible)
{
	if (Target)
	{
		Target->SetVisibility(Visible);
	}
}

void UPlayerAttackComponent::Fire()
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

void UPlayerAttackComponent::ToggleWeapon()
{
	switch(EquipWeapon)
	{
	case EEquipWeapon::Rifle:
		EquipWeapon = EEquipWeapon::Sniper;
		break;
	case EEquipWeapon::Sniper:
	case EEquipWeapon::None:
	default:
		EquipWeapon = EEquipWeapon::Rifle;
		ExitSniper();
		break;
	}
}

bool UPlayerAttackComponent::EnterSniper()
{
	if (EquipWeapon != EEquipWeapon::Sniper) return false;
	if (GetCastedCharacter())
	{
		GetCastedCharacter()->SetCameraFOV(InSniperFov);
	}
	return true;
}

bool UPlayerAttackComponent::ExitSniper()
{
	if (EquipWeapon != EEquipWeapon::Sniper) return false;
	if (GetCastedCharacter())
	{
		GetCastedCharacter()->SetCameraFOV(BaseFov);
	}
	return true;
}

float UPlayerAttackComponent::GetBaseFOV()
{
	return BaseFov;
}

EEquipWeapon UPlayerAttackComponent::GetEquipWeapon() const
{
	return EquipWeapon;
}

ABaseCharacter* UPlayerAttackComponent::GetCastedCharacter()
{
	if (!CastedOwnerCharacter)
	{
		CastedOwnerCharacter = Cast<ABaseCharacter>(OwnerCharacter);
	}
	return CastedOwnerCharacter;
}
