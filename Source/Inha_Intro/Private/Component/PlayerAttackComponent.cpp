// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/PlayerAttackComponent.h"

#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "Component/EnemyFSM.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Object/BaseWeapon.h"
#include "Player/BaseCharacter.h"
#include "Projectile/BaseBullet.h"

UPlayerAttackComponent::UPlayerAttackComponent()
{
	
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
	if (Weapons.IsEmpty()) return;
	
	if (Weapons[CurrentWeaponIndex]) 
	{
		Weapons[CurrentWeaponIndex]->Fire();
		OnRifleFire.ExecuteIfBound();
	}
}

void UPlayerAttackComponent::ToggleWeapon()
{
	if (Weapons.IsEmpty()) return;
	
	Weapons[CurrentWeaponIndex]->SetActorHiddenInGame(true);
	
	++CurrentWeaponIndex;

	if (Weapons.Num() <= CurrentWeaponIndex) CurrentWeaponIndex = 0;
	
	Weapons[CurrentWeaponIndex]->SetActorHiddenInGame(false);
}

bool UPlayerAttackComponent::EnterSniper()
{
	if (Weapons.IsEmpty() || !Weapons[CurrentWeaponIndex])
	{
		return false;
	}
	return Weapons[CurrentWeaponIndex]->EnterSniper();
}

bool UPlayerAttackComponent::ExitSniper()
{
	if (Weapons.IsEmpty() || !Weapons[CurrentWeaponIndex])
	{
		return false;
	}
	return Weapons[CurrentWeaponIndex]->ExitSniper();
}

int UPlayerAttackComponent::WeaponNum()
{
	if (Weapons.IsEmpty()) return 0;
	return Weapons.Num();
}

void UPlayerAttackComponent::AddWeapon(ABaseWeapon* TargetWeapon)
{
	Weapons.AddUnique(TargetWeapon);
}

void UPlayerAttackComponent::RemoveWeapon(ABaseWeapon* TargetWeapon)
{
	Weapons.Remove(TargetWeapon);
}

EWeaponType UPlayerAttackComponent::GetWeaponType()
{
	if (Weapons.IsEmpty() || !Weapons[CurrentWeaponIndex])
	{
		return EWeaponType::None;
	}
	return Weapons[CurrentWeaponIndex]->GetWeaponType();
}

USkeletalMeshComponent* UPlayerAttackComponent::GetWeaponSkeletalMesh()
{
	if (Weapons.IsEmpty() || !Weapons[CurrentWeaponIndex])
	{
		return nullptr;
	}
	return Weapons[CurrentWeaponIndex]->GetMesh();
}

ABaseCharacter* UPlayerAttackComponent::GetCastedCharacter()
{
	if (!CastedOwnerCharacter)
	{
		CastedOwnerCharacter = Cast<ABaseCharacter>(OwnerCharacter);
	}
	return CastedOwnerCharacter;
}
