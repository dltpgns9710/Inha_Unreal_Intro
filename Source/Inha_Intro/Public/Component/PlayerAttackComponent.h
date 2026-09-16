// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Component/PlayerBaseComponent.h"
#include "Object/BaseWeapon.h"
#include "PlayerAttackComponent.generated.h"

class ABaseWeapon;
class ABaseCharacter;
class UNiagaraSystem;
class ABaseBullet;

DECLARE_DELEGATE(FOnFire)

/**
 * 
 */
UCLASS()
class INHA_INTRO_API UPlayerAttackComponent : public UPlayerBaseComponent
{
	GENERATED_BODY()
	
public:
	UPlayerAttackComponent();
	
private:
	void SetSkeletalMeshVisibility(USkeletalMeshComponent* Target, bool Visible);
	
	UPROPERTY()
	TObjectPtr<ABaseCharacter> CastedOwnerCharacter;
	ABaseCharacter* GetCastedCharacter();
	
	UPROPERTY()
	TArray<ABaseWeapon*> Weapons;
	
	int CurrentWeaponIndex = 0;
	
public:
	void Fire();
	void ToggleWeapon();
	bool EnterSniper();
	bool ExitSniper();
	int WeaponNum();
	void AddWeapon(ABaseWeapon* TargetWeapon);
	void RemoveWeapon(ABaseWeapon* TargetWeapon);
	
	UFUNCTION(blueprintcallable)
	EWeaponType GetWeaponType();
	
	USkeletalMeshComponent* GetWeaponSkeletalMesh();
	FOnFire OnRifleFire;
};
