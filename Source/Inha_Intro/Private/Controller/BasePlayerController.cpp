// Fill out your copyright notice in the Description page of Project Settings.

#include "Public/Controller/BasePlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Blueprint/UserWidget.h"
#include "Component/PlayerMovementComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/BaseCharacter.h"
#include "Projectile/BaseBullet.h"
#include "UI/CrosshairHUD.h"

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

void ABasePlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	UpdateCrossHair();
}

void ABasePlayerController::Input_Look(const FInputActionValue& InputActionValue)
{
	if (OwnerCharacter == nullptr || 
		GetPlayerMovementComponent() == nullptr)
		return;
	
	GetPlayerMovementComponent()->Look(InputActionValue);
}

void ABasePlayerController::Input_Move(const FInputActionValue& InputActionValue)
{
	if (OwnerCharacter == nullptr || 
		GetPlayerMovementComponent() == nullptr)
		return;

	GetPlayerMovementComponent()->Move(InputActionValue);
}

void ABasePlayerController::Input_Jump(const FInputActionValue& InputActionValue)
{
	if (OwnerCharacter == nullptr || 
		GetPlayerMovementComponent() == nullptr)
		return;
	
	GetPlayerMovementComponent()->Jump(InputActionValue);
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
		if (CrossHairWidget)
		{
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
	if (OwnerCharacter == nullptr || 
		GetPlayerMovementComponent() == nullptr)
		return;
	
	GetPlayerMovementComponent()->Run_Enter(InputActionValue);
}

void ABasePlayerController::Input_ExitRun(const FInputActionValue& InputActionValue)
{
	if (OwnerCharacter == nullptr || 
		GetPlayerMovementComponent() == nullptr)
		return;
	
	GetPlayerMovementComponent()->Run_Exit(InputActionValue);
}

void ABasePlayerController::UpdateCrossHair()
{
	// 크로스헤어가 그려지고 있는 HUD 가져오기
	ACrosshairHUD* HUD = Cast<ACrosshairHUD>(GetHUD());
	if (HUD == nullptr)
		return;

	// 1. 이동이 있으면 크로스헤어 벌어진다.
	// Velocity 값 기준으로 얼만큼 벌어지게 할지 결정.
	// 최대 이속 대비, 현재 이속값을 계산해서 얼마나 크로스헤어가 벌어지는지 계산
	FVector2D WalkSpeedRange(0.f, GetPlayerMovementComponent()->GetMaxWalkSpeed());
	FVector2D VelocityMulRange(0.f, 1.f);
	FVector Velocity = GetCastOwnerCharacter()->GetVelocity();
	Velocity.Z = 0; // 점프속도는 무시

	// 최대속도 600, 현재 이속 : 300 -> 0.5f 라는 수치를 얻고싶다.
	// 최대속도 600, 현재 이속 : 600 -> 1.0f
	float CrosshairVelocityAlpha = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMulRange, Velocity.Size());

	// 환산된값 0~1 사이의 기준으로, Weapon Min,Max 값에 대응하는 값을 뽑는다.
	// min :2, max:6, 2~6 사이의 적절한값으로 환산해준다.
	float CrosshairVelocityFactor = FMath::Lerp(CrosshairSpreadMin, CrosshairSpreadMax, CrosshairVelocityAlpha);

	// 최종값 : 이동 Factor + Fire Factor
	float CrosshairFireFactor = 0;	//@TODO : 총기마다 벌어지는 크로스헤어 값
	float TotalCrosshairFactor = CrosshairVelocityFactor + CrosshairFireFactor;

	// 최종적으로 HUD 객체에 Spread 정보를 전달한다.
	HUD->SetCrosshairSpread(TotalCrosshairFactor);
}

ABaseCharacter* ABasePlayerController::GetCastOwnerCharacter()
{
	if (!CastOwnerCharacter)
	{
		CastOwnerCharacter = Cast<ABaseCharacter>(OwnerCharacter);
	}
	return CastOwnerCharacter;
}

UPlayerMovementComponent* ABasePlayerController::GetPlayerMovementComponent()
{
	if (GetCastOwnerCharacter() && !PlayerMovementComponent)
	{
		PlayerMovementComponent = Cast<UPlayerMovementComponent>(GetCastOwnerCharacter()->GetPlayerMovementComponent());
	}
	return PlayerMovementComponent;
}
