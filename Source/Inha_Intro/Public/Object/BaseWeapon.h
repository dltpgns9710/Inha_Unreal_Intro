// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseWeapon.generated.h"

class USphereComponent;
class ABaseCharacter;
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Rifle,
	Sniper,
	Pistol,
	None
};

UCLASS(Abstract)
class INHA_INTRO_API ABaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseWeapon();

	bool virtual EnterSniper();
	bool virtual ExitSniper();
	virtual void Fire();
	void LinkAnimLayer();
	void UnlinkAnimLayer();
	USkeletalMeshComponent* GetMesh();
	EWeaponType GetWeaponType();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY()
	TObjectPtr<ABaseCharacter> CastedOwnerCharacter;
	ABaseCharacter* GetCastedCharacter();
	
	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	TSubclassOf<UCameraShakeBase> CameraShake;
	
private:
	UPROPERTY(VisibleAnywhere, Category = "Weapon", meta = (AllowPrivateAccess = true))
	TObjectPtr<USkeletalMeshComponent> WeaponMesh = nullptr;

	UPROPERTY(VisibleAnywhere, Category="Weapon");
	TObjectPtr<USphereComponent> CollisionComp;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	EWeaponType WeaponType = EWeaponType::Rifle;
	
	UPROPERTY(EditAnywhere, Category = "Weapon")
	TObjectPtr<UAnimationAsset> FireWeaponAnimation;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<UAnimInstance> WeaponLayerClass;
	
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult);
};
