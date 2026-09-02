// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/BaseCharacterAnimInstance.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UBaseCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	APawn* OwningPawn = TryGetPawnOwner();
	Character = Cast<ACharacter>(OwningPawn);
	if (Character)
	{
		MovementComponent = Character->GetCharacterMovement();
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
}
