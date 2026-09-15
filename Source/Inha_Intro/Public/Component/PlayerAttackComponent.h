// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Component/PlayerBaseComponent.h"
#include "PlayerAttackComponent.generated.h"

class ABaseCharacter;
class UNiagaraSystem;
class ABaseBullet;

UENUM(BlueprintType)
enum class EEquipWeapon : uint8
{
	Rifle,
	Sniper,
	None
};

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
	UPROPERTY(EditDefaultsOnly, Category = "Factory")
	TSubclassOf<ABaseBullet> BulletFactory;
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	TObjectPtr<UNiagaraSystem> EffectFactory;
	
	UPROPERTY(EditDefaultsOnly, Category = "Values", meta = (AllowPrivateAccess = true))
	EEquipWeapon EquipWeapon = EEquipWeapon::Rifle;
	
	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	TSubclassOf<UCameraShakeBase> CameraShake;
	
	UPROPERTY(EditDefaultsOnly, Category = "Values")
	float InSniperFov = 45.f;
	UPROPERTY(EditDefaultsOnly, Category = "Values")
	float BaseFov = 90.f;
	UPROPERTY(EditDefaultsOnly, Category = "Values")
	float SniperLength = 5000.f;
	
	void FireRifle();
	void FireSniper();
	void SetSkeletalMeshVisibility(USkeletalMeshComponent* Target, bool Visible);
	
	UPROPERTY()
	TObjectPtr<ABaseCharacter> CastedOwnerCharacter;
	ABaseCharacter* GetCastedCharacter();
	
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	TObjectPtr<UMaterialInterface> BulletDecalMaterial;
	
	UPROPERTY(EditDefaultsOnly, Category = "Effect|Decal")
	FVector DecalSize = FVector(5.f, 5.f, 5.f);
	UPROPERTY(EditDefaultsOnly, Category = "Effect|Decal")
	float DecalLifetime = 5.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	TObjectPtr<UNiagaraSystem> BeamParticles;
	
public:
	void Fire();
	void ToggleWeapon();
	bool EnterSniper();
	bool ExitSniper();
	float GetBaseFOV();
	UFUNCTION(BlueprintCallable)
	EEquipWeapon GetEquipWeapon() const;
	
	FOnFire OnRifleFire;
};
