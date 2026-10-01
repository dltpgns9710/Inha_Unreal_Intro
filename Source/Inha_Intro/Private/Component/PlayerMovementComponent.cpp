// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/PlayerMovementComponent.h"

#include "InputActionValue.h"
#include "Data/Inha_CharacterStats.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/BaseCharacter.h"

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
	Server_RunEnter();
	if (GetCastedCharacter()->IsLocallyControlled())
	{
		if (OwnerMovementComponent == nullptr || 
		GetCastedCharacter() == nullptr || 
		GetCastedCharacter()->GetCharacterStats() == nullptr)
			return;
	
		OwnerMovementComponent->MaxWalkSpeed = GetCastedCharacter()->GetCharacterStats()->RunSpeed;
	}
}

void UPlayerMovementComponent::Run_Exit(const FInputActionValue& InputActionValue)
{
	Server_RunExit();
	if (GetCastedCharacter()->IsLocallyControlled())
	{
		if (OwnerMovementComponent == nullptr || 
		GetCastedCharacter() == nullptr || 
		GetCastedCharacter()->GetCharacterStats() == nullptr)
			return;
	
		OwnerMovementComponent->MaxWalkSpeed = GetCastedCharacter()->GetCharacterStats()->WalkSpeed;
	}
}

float UPlayerMovementComponent::GetMaxWalkSpeed() const
{
	if (OwnerMovementComponent == nullptr)
		return 0;
	
	return OwnerMovementComponent->MaxWalkSpeed;
}

void UPlayerMovementComponent::Server_RunEnter_Implementation()
{
	if (OwnerMovementComponent == nullptr || 
		GetCastedCharacter() == nullptr || 
		GetCastedCharacter()->GetCharacterStats() == nullptr)
		return;
	
	OwnerMovementComponent->MaxWalkSpeed = GetCastedCharacter()->GetCharacterStats()->RunSpeed;
}

void UPlayerMovementComponent::Server_RunExit_Implementation()
{
	if (OwnerMovementComponent == nullptr || 
		GetCastedCharacter() == nullptr || 
		GetCastedCharacter()->GetCharacterStats() == nullptr)
		return;
	
	OwnerMovementComponent->MaxWalkSpeed = GetCastedCharacter()->GetCharacterStats()->WalkSpeed;
}
