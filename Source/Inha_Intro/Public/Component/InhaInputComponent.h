// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "GameplayTagContainer.h"
#include "Data/PlayerInputData.h"
#include "System/MyGameInstance.h"
#include "InhaInputComponent.generated.h"

/**
 * 
 */
UCLASS()
class INHA_INTRO_API UInhaInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()
	
public:
	template <class UserObject, typename CallbackFunc>
	void BindActionByTag(FGameplayTag ActionTag, ETriggerEvent Trigger, UserObject* ContextObject, CallbackFunc Func);

};

template <class UserObject, typename CallbackFunc>
void UInhaInputComponent::BindActionByTag(FGameplayTag ActionTag, ETriggerEvent Trigger, UserObject* ContextObject, CallbackFunc Func)
{
	UMyGameInstance* CastedGameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
	if (!CastedGameInstance) return;
	
	UPlayerInputData* InputDataAsset = CastedGameInstance->InputDataAsset;
	if (!InputDataAsset) return;
	
	if (InputDataAsset->GetInputActionByTag(ActionTag))
	{
		BindAction(InputDataAsset->GetInputActionByTag(ActionTag), Trigger, ContextObject, Func);
	}
}
