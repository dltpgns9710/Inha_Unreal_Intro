// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BasePlayerController.generated.h"

class UInputAction;
class UInputMappingContext;
struct FInputActionValue;
/**
 * 
 */
UCLASS()
class INHA_INTRO_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> IMC;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Look;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Move;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Jump;
private:
	UPROPERTY()
	TObjectPtr<ACharacter> OwnerCharacter = nullptr;
	
	void Input_Look(const FInputActionValue& InputActionValue);
	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_Jump(const FInputActionValue& InputActionValue);
};
