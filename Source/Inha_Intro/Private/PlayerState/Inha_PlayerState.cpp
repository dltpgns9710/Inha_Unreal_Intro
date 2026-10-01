// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerState/Inha_PlayerState.h"

#include "Data/Inha_CharacterStats.h"
#include "Net/UnrealNetwork.h"
#include "Player/BaseCharacter.h"

void AInha_PlayerState::OnRep_Xp(int32 OldValue) const
{
	OnXpChanged.Broadcast(Xp);
}

void AInha_PlayerState::OnRep_Level(int32 OldValue) const
{
	OnLevelChanged.Broadcast(Level);
}

void AInha_PlayerState::AddXp(int32 Value)
{
	if (!HasAuthority()) return;

	Xp += Value;
	OnXpChanged.Broadcast(Xp);
	
	if (const auto Character = Cast<ABaseCharacter>(GetPawn()))
	{
		if (Character->GetCharacterStats()->NextLevelXp < Xp)
		{
			Level++;
			Character->UpdateCharacterStats(Level);
			OnLevelChanged.Broadcast(Level);
		}
	}
	ForceNetUpdate();
}

void AInha_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION(AInha_PlayerState, Xp, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AInha_PlayerState, Level, COND_OwnerOnly);
}
