// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/PlayerInputData.h"

UInputMappingContext* UPlayerInputData::GetIMC()
{
	return BaseImc;
}

UInputAction* UPlayerInputData::GetInputActionByTag(FGameplayTag FindTag)
{
	for(FInputActionData InputAction : InputActions)
	{
		if (InputAction.InputTag.MatchesTagExact(FindTag)) return InputAction.InputAction;
	}
	return nullptr;
}
