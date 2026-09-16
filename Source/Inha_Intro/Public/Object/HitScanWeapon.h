// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/BaseWeapon.h"
#include "HitScanWeapon.generated.h"

class UNiagaraSystem;
/**
 * 
 */
UCLASS()
class INHA_INTRO_API AHitScanWeapon : public ABaseWeapon
{
	GENERATED_BODY()

public:
	bool virtual EnterSniper() override;
	bool virtual ExitSniper() override;
	virtual void Fire() override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Values")
	float InSniperFov = 45.f;
	UPROPERTY(EditDefaultsOnly, Category = "Values")
	float BaseFov = 90.f;
	UPROPERTY(EditDefaultsOnly, Category = "Values")
	float SniperLength = 5000.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	TObjectPtr<UNiagaraSystem> EffectFactory;
	
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	TObjectPtr<UNiagaraSystem> BeamParticles;
	
	UPROPERTY(EditDefaultsOnly, Category = "Effect|Decal")
	TObjectPtr<UMaterialInterface> BulletDecalMaterial;
	UPROPERTY(EditDefaultsOnly, Category = "Effect|Decal")
	FVector DecalSize = FVector(5.f, 5.f, 5.f);
	UPROPERTY(EditDefaultsOnly, Category = "Effect|Decal")
	float DecalLifetime = 5.f;
};
