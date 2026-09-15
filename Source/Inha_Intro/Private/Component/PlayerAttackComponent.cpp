// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/PlayerAttackComponent.h"

#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "Component/EnemyFSM.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"
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
	
	FVector FireLocation = FVector::ZeroVector;
	if (GetCastedCharacter())
	{
		FireLocation = GetCastedCharacter()->GetSniperFireLocation();
	}
	
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitInfo, StartPosition, EndPosition, ECC_GameTraceChannel1, CollisionParams);
	if (bHit)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			EffectFactory,
			HitInfo.ImpactPoint,
			HitInfo.ImpactNormal.Rotation()
		);
		
		UDecalComponent* Decal = UGameplayStatics::SpawnDecalAtLocation(
				GetWorld(),
				BulletDecalMaterial,
				DecalSize,
				HitInfo.ImpactPoint,
				HitInfo.ImpactNormal.Rotation(),
				DecalLifetime);
		
		if (Decal)
		{
			Decal->SetFadeScreenSize(0);
		}
		
		if(BeamParticles)
		{
			UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(),
				BeamParticles,  // UNiagaraSystem* 타입
				FireLocation,
				FRotator::ZeroRotator,
				FVector(1.0f, 1.0f, 1.0f),  // Scale
				true,  // AutoDestroy
				true,  // AutoActivate
				ENCPoolMethod::AutoRelease  // Pooling 방식
			);

			UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(
				NiagaraComp,
				FName("ImpactPositions"),  // Niagara 변수 이름
				TArray<FVector>({ HitInfo.ImpactPoint })  // ImpactPoint를 포함하는 배열
			);

			NiagaraComp->SetVariableBool(FName(TEXT("Trigger")), true);
		}
		
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
