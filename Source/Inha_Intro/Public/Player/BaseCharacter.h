// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

DECLARE_DELEGATE(FOnTrigger)
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDynamicTrigger);

class UPlayerAttackComponent;
class UPlayerBaseComponent;
class UNiagaraSystem;
class ABaseBullet;
struct FInputActionValue;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class INHA_INTRO_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
private:
	UPROPERTY(VisibleAnywhere, Category = "Component")
	TObjectPtr<UPlayerBaseComponent> PlayerMovementComponent = nullptr;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly ,Category = "Component", meta=(AllowPrivateAccess=true))
	TObjectPtr<UPlayerBaseComponent> PlayerAttackComponent = nullptr;
	
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	TObjectPtr<USkeletalMeshComponent> RifleMesh = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	TObjectPtr<USkeletalMeshComponent> SniperMesh = nullptr;
	
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> FireRifleMontage;
	
	UPROPERTY()
	TObjectPtr<APlayerController> CachedPlayerController = nullptr;
	APlayerController* GetPlayerController();
	
	UPROPERTY()
	TObjectPtr<UPlayerAttackComponent> CastedAttackComponent;
	UPlayerAttackComponent* GetCastedAttackComponent();
	
	UPROPERTY(EditAnywhere, Category = "Values")
	float HiddenMeshDist = 200.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health", meta=(AllowPrivateAccess=true))
	float Hp;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health", meta=(AllowPrivateAccess=true))
	float InitHp = 3;
	
	UPROPERTY(BlueprintAssignable, meta=( AllowPrivateAccess=true))
	FOnDynamicTrigger OnWeaponChange;
	
	UPROPERTY(BlueprintAssignable, meta=( AllowPrivateAccess=true))
	FOnDynamicTrigger OnGameOver;
	
	void GameOver();
	
	void SetSkeletalMeshVisibility(USkeletalMeshComponent* Target, bool Visible);
	
	void OnFireCallback();
	
	float AO_StartYaw = 0;
	float AO_Yaw = 0;
	float AO_Pitch = 0;
	void UpdateAimOffset(float DeltaTime);
	
public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FOnTrigger OnFire;
	
	UFUNCTION(BlueprintCallable, Category = "Health")
	void OnHitEvent();
	
	void Fire();
	void ToggleWeapon();
	bool EnterSniper();
	bool ExitSniper();
	void SetCameraFOV(float NewFOV) const;
	float GetAoYaw();
	float GetAoPitch();
	FTransform GetFireTransform() const;
	FVector GetSniperFireLocation() const;
	FVector GetCameraLocation() const;
	FVector GetCameraForwardVector() const;
	
	UPlayerBaseComponent* GetPlayerMovementComponent() const;
	UPlayerBaseComponent* GetPlayerAttackComponent() const;
	USkeletalMeshComponent* GetWeaponMesh();
};
