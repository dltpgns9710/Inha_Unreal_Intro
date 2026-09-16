// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/HitScanWeapon.h"

#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "Component/EnemyFSM.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/BaseCharacter.h"

bool AHitScanWeapon::EnterSniper()
{
	if (GetCastedCharacter())
	{
		GetCastedCharacter()->SetCameraFOV(InSniperFov);
	}
	return true;
}

bool AHitScanWeapon::ExitSniper()
{
	if (GetCastedCharacter())
	{
		GetCastedCharacter()->SetCameraFOV(BaseFov);
	}
	return true;
}

void AHitScanWeapon::Fire()
{
	if (!GetCastedCharacter()) return;
	
	FHitResult HitInfo;
	FCollisionQueryParams CollisionParams;
	FVector StartPosition= GetCastedCharacter()->GetCameraLocation();
	FVector EndPosition= StartPosition + GetCastedCharacter()->GetCameraForwardVector() * SniperLength;
	
	APlayerController* PlayerController = Cast<APlayerController>(GetCastedCharacter()->GetController());
	if (PlayerController && CameraShake)
	{
		PlayerController->PlayerCameraManager->StartCameraShake(CameraShake);
	}
	
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
