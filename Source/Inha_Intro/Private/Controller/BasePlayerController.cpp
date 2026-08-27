// Fill out your copyright notice in the Description page of Project Settings.

#include "Public/Controller/BasePlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"

void ABasePlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	OwnerCharacter = GetCharacter();
}

void ABasePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem)
	{
		Subsystem->AddMappingContext(IMC, 0);
	}
	
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	if (EnhancedInputComponent)
	{
		EnhancedInputComponent->BindAction(IA_Look, ETriggerEvent::Triggered, this, &ABasePlayerController::Input_Look);
		EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Triggered, this, &ABasePlayerController::Input_Move);
		EnhancedInputComponent->BindAction(IA_Jump, ETriggerEvent::Triggered, this, &ABasePlayerController::Input_Jump);
	}
}

void ABasePlayerController::Input_Look(const FInputActionValue& InputActionValue)
{
	if (OwnerCharacter == nullptr)
		return;

	const FVector2D Value = InputActionValue.Get<FVector2D>();

	if (Value.X != 0.0f)
	{
		OwnerCharacter->AddControllerYawInput(Value.X);
	}

	if (Value.Y != 0.0f)
	{
		OwnerCharacter->AddControllerPitchInput(Value.Y);
	}
}

void ABasePlayerController::Input_Move(const FInputActionValue& InputActionValue)
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

void ABasePlayerController::Input_Jump(const FInputActionValue& InputActionValue)
{
	if (OwnerCharacter == nullptr)
		return;
	
	const bool Value = InputActionValue.Get<bool>();
	
	if (Value)
	{
		OwnerCharacter->Jump();
	}
}
