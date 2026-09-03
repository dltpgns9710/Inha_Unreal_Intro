// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/EnemyAnimation.h"

#include "Character/Enemy.h"

void UEnemyAnimation::SetAnimState(EEnemyState State)
{
	AnimState = State;
}

void UEnemyAnimation::SetCanAttackPlay(bool bCanAttack)
{
	bCanAttackPlay = bCanAttack;
}

void UEnemyAnimation::OnDamaged(FName SectionName)
{
	OnDamageCallback(SectionName);
}

bool UEnemyAnimation::IsDieDone() const
{
	return bDieDone;
}

bool UEnemyAnimation::IsAttackPlaying() const
{
	return bIsAttackPlaying;
}

void UEnemyAnimation::OnEndAttackCallback()
{
	SetCanAttackPlay(false);
}

AEnemy* UEnemyAnimation::GetCastedOwner()
{
	if (!CachedOwner)
	{
		CachedOwner = Cast<AEnemy>(TryGetPawnOwner());
	}
	return CachedOwner;
}
