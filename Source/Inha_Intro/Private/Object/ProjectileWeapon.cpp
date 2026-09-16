// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/ProjectileWeapon.h"

#include "Player/BaseCharacter.h"
#include "Projectile/BaseBullet.h"

void AProjectileWeapon::Fire()
{
	if (!GetCastedCharacter()) return;
	
	FTransform FirePosition = GetCastedCharacter()->GetFireTransform();
	GetWorld()->SpawnActor<ABaseBullet>(BulletFactory, FirePosition);
	
	// 카메라 셰이크 재생
	APlayerController* PlayerController = Cast<APlayerController>(GetCastedCharacter()->GetController());
	if (PlayerController && CameraShake)
	{
		PlayerController->PlayerCameraManager->StartCameraShake(CameraShake);
	}
}
