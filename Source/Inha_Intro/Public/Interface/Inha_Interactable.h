// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Inha_Interactable.generated.h"

class ABaseCharacter;
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInha_Interactable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class INHA_INTRO_API IInha_Interactable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interaction", meta=(DisplayName = "Interact"))
	void Interact(ABaseCharacter* CharacterInstigator);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interaction", meta=(DisplayName = "Can Interact"))
	bool CanInteract(ABaseCharacter* CharacterInstigator) const;
};
