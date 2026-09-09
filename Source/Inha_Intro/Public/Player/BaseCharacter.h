// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

class UPlayerAttackComponent;
class UPlayerBaseComponent;
DECLARE_DELEGATE(FOnTrigger)

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

private:
	UPROPERTY(VisibleAnywhere, Category = "Component")
	TObjectPtr<UPlayerBaseComponent> PlayerMovementComponent = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "Component")
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
	
	void SetSkeletalMeshVisibility(USkeletalMeshComponent* Target, bool Visible);
	
	void OnFireCallback();
	
public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FOnTrigger OnFire;
	
	void Fire();
	void ToggleWeapon();
	bool EnterSniper();
	bool ExitSniper();
	void SetCameraFOV(float NewFOV) const;
	FTransform GetFireTransform() const;
	FVector GetCameraLocation() const;
	FVector GetCameraForwardVector() const;
	
	UPlayerBaseComponent* GetPlayerMovementComponent() const;
	UPlayerBaseComponent* GetPlayerAttackComponent() const;
};
