// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/BaseWeapon.h"
#include "ProjectileWeapon.generated.h"

class ABaseBullet;
/**
 * 
 */
UCLASS()
class INHA_INTRO_API AProjectileWeapon : public ABaseWeapon
{
	GENERATED_BODY()
	
public:
	virtual void Fire() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Factory")
	TSubclassOf<ABaseBullet> BulletFactory;
};
