// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BaseCharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class INHA_INTRO_API UBaseCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	TObjectPtr<class ACharacter> Character = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	TObjectPtr<class UCharacterMovementComponent> MovementComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	FVector Velocity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	float GroundSpeed = 0;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	float Direction = 0;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	bool bShouldMove = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	bool bIsFalling = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	bool bIsFire = false;
	
	UPROPERTY(EditDefaultsOnly, Category = PlayerAnim)
	float FireRecoveryTime = 1.5f;
	
private:
	void OnFire();
	
	FTimerHandle OnFireTimer;
};
