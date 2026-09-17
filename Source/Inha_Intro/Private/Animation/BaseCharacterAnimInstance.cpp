// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/BaseCharacterAnimInstance.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/BaseCharacter.h"

void UBaseCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	APawn* OwningPawn = TryGetPawnOwner();
	Character = Cast<ACharacter>(OwningPawn);
	if (Character)
	{
		MovementComponent = Character->GetCharacterMovement();
	}
	CastCharacter = Cast<ABaseCharacter>(OwningPawn);
	if (CastCharacter)
	{
		CastCharacter->OnFire.BindUObject(this, &ThisClass::OnFire);
	}
}

void UBaseCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if (Character && MovementComponent)
	{
		// 속도 계산
		Velocity = Character->GetVelocity();
		GroundSpeed = Velocity.Length();
		bShouldMove = (MovementComponent->GetCurrentAcceleration().Size() > 0) && (GroundSpeed >= 0.01f);
		bIsFalling = MovementComponent->IsFalling();
		
		// 방향 계산
		Direction = CalculateDirection(Velocity, Character->GetActorRotation());
		
	}
	if (CastCharacter)
	{
		AO_Yaw = CastCharacter->GetAoYaw();
		AO_Pitch = CastCharacter->GetAoPitch();
		bIsUnarmed = CastCharacter->IsUnarmed();
	}
	//if (AWeapon* Weapon = Character->playerFire->Weapon)
	if (CastCharacter && CastCharacter->GetWeaponMesh())
	{
		// 무기의 LeftHandSocket 위치를 월드 좌표로 가져온다.
		FTransform LeftHandWorldTransform = CastCharacter->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);

		FVector OutPosition;
		FRotator OutRotator;
		Character->GetMesh()->TransformToBoneSpace(FName("hand_r"),
			LeftHandWorldTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotator);

		// hand_r 공간으로 변환한 좌표를 animation 좌표로 설정한다.
		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotator));
	}
}

void UBaseCharacterAnimInstance::OnFire()
{
	bIsFire = true;

	if (GetWorld()->GetTimerManager().IsTimerActive(OnFireTimer))
	{
		GetWorld()->GetTimerManager().ClearTimer(OnFireTimer);
	}
	
	GetWorld()->GetTimerManager().SetTimer(
		OnFireTimer,
		FTimerDelegate::CreateLambda([this]()
		{
			bIsFire = false;
		}),
		FireRecoveryTime,
		false
	);
}

