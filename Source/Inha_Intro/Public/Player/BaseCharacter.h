// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

struct FInputActionValue;
class UCameraComponent;
class USpringArmComponent;

UENUM()
enum class EEquipWeapon : uint8
{
	Rifle,
	Sniper,
	None
};

UCLASS()
class INHA_INTRO_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera = nullptr;
	
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	TObjectPtr<USkeletalMeshComponent> RifleMesh = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	TObjectPtr<USkeletalMeshComponent> SniperMesh = nullptr;
	
	UPROPERTY(EditAnywhere, Category = "Values")
	float HiddenMeshDist = 200.f;
	UPROPERTY(EditDefaultsOnly, Category = "Values")
	float InSniperFov = 45.f;
	UPROPERTY(EditDefaultsOnly, Category = "Values")
	float BaseFov = 90.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Values", meta = (AllowPrivateAccess = true))
	EEquipWeapon EquipWeapon = EEquipWeapon::Rifle;
	
	void SetSkeletalMeshVisibility(USkeletalMeshComponent* Target, bool Visible);
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void ToggleWeapon();
	bool EnterSniper();
	bool ExitSniper();
	FTransform GetFirePosition() const;
	EEquipWeapon GetEquipWeapon() const;
};
