// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Component/PlayerBaseComponent.h"
#include "PlayerMovementComponent.generated.h"

struct FInputActionValue;
/**
 * 
 */
UCLASS()
class INHA_INTRO_API UPlayerMovementComponent : public UPlayerBaseComponent
{
	GENERATED_BODY()
	
public:
	void Look(const FInputActionValue& InputActionValue);
	void Move(const FInputActionValue& InputActionValue);
	void Jump(const FInputActionValue& InputActionValue);
	void Run_Enter(const FInputActionValue& InputActionValue);
	void Run_Exit(const FInputActionValue& InputActionValue);
	
	float GetWalkSpeed() const;
	float GetRunSpeed() const;
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Values")
	float WalkSpeed = 300.f;
	UPROPERTY(EditDefaultsOnly, Category = "Values")
	float RunSpeed = 600.f;
};
