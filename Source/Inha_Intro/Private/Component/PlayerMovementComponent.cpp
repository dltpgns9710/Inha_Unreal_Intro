// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/PlayerMovementComponent.h"

#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UPlayerMovementComponent::Look(const FInputActionValue& InputActionValue)
{
	if (OwnerCharacter == nullptr)
		return;
	
	const FVector2D Value = InputActionValue.Get<FVector2D>();

	//UE_LOG(LogTemp,Warning,TEXT("%f, %f"), Value.X, Value.Y);
	if (Value.X != 0.0f)
	{
		OwnerCharacter->AddControllerYawInput(Value.X);
	}

	if (Value.Y != 0.0f)
	{
		OwnerCharacter->AddControllerPitchInput(-Value.Y);
	}
}

void UPlayerMovementComponent::Move(const FInputActionValue& InputActionValue)
{
	if (OwnerCharacter == nullptr)
		return;
	
	AController* Controller = OwnerCharacter->GetController();

	if (Controller)
	{
		const FVector2D Value = InputActionValue.Get<FVector2D>();
		const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

		if (Value.X != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
			OwnerCharacter->AddMovementInput(MovementDirection, Value.X);
		}

		if (Value.Y != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			OwnerCharacter->AddMovementInput(MovementDirection, Value.Y);
		}
	}
}

void UPlayerMovementComponent::Jump(const FInputActionValue& InputActionValue)
{
	if (OwnerCharacter == nullptr)
		return;
	
	const bool Value = InputActionValue.Get<bool>();
	
	if (Value)
	{
		OwnerCharacter->Jump();
	}
}

void UPlayerMovementComponent::Run_Enter(const FInputActionValue& InputActionValue)
{
	if (OwnerMovementComponent == nullptr)
		return;
	
	OwnerMovementComponent->MaxWalkSpeed = RunSpeed;
}

void UPlayerMovementComponent::Run_Exit(const FInputActionValue& InputActionValue)
{
	if (OwnerMovementComponent == nullptr)
		return;
	
	OwnerMovementComponent->MaxWalkSpeed = WalkSpeed;
}

float UPlayerMovementComponent::GetWalkSpeed() const
{
	return WalkSpeed;
}

float UPlayerMovementComponent::GetRunSpeed() const
{
	return RunSpeed;
}
