// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "PlayerInputData.generated.h"

class UInputAction;
class UInputMappingContext;

USTRUCT()
struct FInputActionData
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputTag;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> InputAction;
};
/**
 * 
 */
UCLASS()
class INHA_INTRO_API UPlayerInputData : public UDataAsset
{
	GENERATED_BODY()
private:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputMappingContext> BaseImc;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<FInputActionData> InputActions;
public:
	UInputMappingContext* GetIMC();
	UInputAction* GetInputActionByTag(FGameplayTag FindTag);
};
