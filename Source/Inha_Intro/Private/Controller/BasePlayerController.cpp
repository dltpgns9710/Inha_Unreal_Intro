// Fill out your copyright notice in the Description page of Project Settings.

#include "Public/Controller/BasePlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/BaseCharacter.h"
#include "Projectile/BaseBullet.h"

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
		EnhancedInputComponent->BindAction(IA_Jump, ETriggerEvent::Started, this, &ABasePlayerController::Input_Jump);
		EnhancedInputComponent->BindAction(IA_Fire, ETriggerEvent::Started, this, &ABasePlayerController::Input_Fire);
		EnhancedInputComponent->BindAction(IA_WeaponToggle, ETriggerEvent::Started, this, &ABasePlayerController::Input_WeaponToggle);
		EnhancedInputComponent->BindAction(IA_Sniper, ETriggerEvent::Started, this, &ABasePlayerController::Input_EnterSniper);
		EnhancedInputComponent->BindAction(IA_Sniper, ETriggerEvent::Completed, this, &ABasePlayerController::Input_ExitSniper);
		EnhancedInputComponent->BindAction(IA_Run, ETriggerEvent::Started, this, &ABasePlayerController::Input_EnterRun);
		EnhancedInputComponent->BindAction(IA_Run, ETriggerEvent::Completed, this, &ABasePlayerController::Input_ExitRun);
	}
}

void ABasePlayerController::Input_Look(const FInputActionValue& InputActionValue)
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

void ABasePlayerController::Input_Fire(const FInputActionValue& InputActionValue)
{
	if (GetCastOwnerCharacter())
	{
		GetCastOwnerCharacter()->Fire();
	}
}

void ABasePlayerController::Input_WeaponToggle(const FInputActionValue& InputActionValue)
{
	if (GetCastOwnerCharacter())
	{
		if (GetCastOwnerCharacter()->GetEquipWeapon() == EEquipWeapon::Sniper && CrossHairWidget)
		{
			GetCastOwnerCharacter()->ExitSniper();
			CrossHairWidget->RemoveFromParent();
			CrossHairWidget = nullptr;
		}
		GetCastOwnerCharacter()->ToggleWeapon();
	}
}

void ABasePlayerController::Input_EnterSniper(const FInputActionValue& InputActionValue)
{
	if (GetCastOwnerCharacter())
	{
		if (GetCastOwnerCharacter()->EnterSniper())
		{
			CrossHairWidget = CreateWidget<UUserWidget>(this, CrossHairWidgetClass);
			if (CrossHairWidget)
			{
				CrossHairWidget->AddToViewport();
			}
		}
	}
}

void ABasePlayerController::Input_ExitSniper(const FInputActionValue& InputActionValue)
{
	if (GetCastOwnerCharacter())
	{
		if (GetCastOwnerCharacter()->ExitSniper() && CrossHairWidget)
		{
			CrossHairWidget->RemoveFromParent();
			CrossHairWidget = nullptr;
		}
	}
}

void ABasePlayerController::Input_EnterRun(const FInputActionValue& InputActionValue)
{
	GetCastOwnerCharacter()->SetSpeedToRun();
}

void ABasePlayerController::Input_ExitRun(const FInputActionValue& InputActionValue)
{
	GetCastOwnerCharacter()->SetSpeedToWalk();
}

ABaseCharacter* ABasePlayerController::GetCastOwnerCharacter()
{
	if (!CastOwnerCharacter)
	{
		CastOwnerCharacter = Cast<ABaseCharacter>(OwnerCharacter);
	}
	return CastOwnerCharacter;
}
