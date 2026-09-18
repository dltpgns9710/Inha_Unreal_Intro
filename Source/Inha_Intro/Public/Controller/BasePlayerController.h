// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "GameplayTagContainer.h"
#include "Data/PlayerInputData.h"
#include "GameFramework/PlayerController.h"
#include "BasePlayerController.generated.h"

class UPlayerInputData;
class UPlayerMovementComponent;
class ABaseCharacter;
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
	virtual void Tick(float DeltaSeconds) override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UPlayerInputData> InputDataAsset;
	
private:
	UPROPERTY()
	TObjectPtr<ACharacter> OwnerCharacter = nullptr;
	UPROPERTY()
	TObjectPtr<ABaseCharacter> CastOwnerCharacter = nullptr;
	UPROPERTY()
	TObjectPtr<UUserWidget> CrossHairWidget = nullptr;
	UPROPERTY()
	TObjectPtr<UPlayerMovementComponent> PlayerMovementComponent = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> CrossHairWidgetClass;
	
	void Input_Look(const FInputActionValue& InputActionValue);
	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_Jump(const FInputActionValue& InputActionValue);
	void Input_Fire(const FInputActionValue& InputActionValue);
	void Input_WeaponToggle(const FInputActionValue& InputActionValue);
	void Input_EnterSniper(const FInputActionValue& InputActionValue);
	void Input_ExitSniper(const FInputActionValue& InputActionValue);
	void Input_EnterRun(const FInputActionValue& InputActionValue);
	void Input_ExitRun(const FInputActionValue& InputActionValue);
	
	template <class UserObject, typename CallbackFunc>
	void BindActionByTag(UEnhancedInputComponent* EnhancedInputComponent, FGameplayTag ActionTag, ETriggerEvent Trigger, UserObject* ContextObject, CallbackFunc Func);
	
	UPROPERTY(EditAnywhere, Category = "IHGame|Value")
	float CrosshairSpreadMax = 6.f;

	UPROPERTY(EditAnywhere, Category = "IHGame|Value")
	float CrosshairSpreadMin = 2.f;

	void UpdateCrossHair();
	
protected:
	UFUNCTION(BlueprintCallable)
	ABaseCharacter* GetCastOwnerCharacter();
	UPlayerMovementComponent* GetPlayerMovementComponent();
};

//원래는 UEnhancedInputComponent 상속받은 클래스에서 하는게 맞음
template <class UserObject, typename CallbackFunc>
void ABasePlayerController::BindActionByTag(UEnhancedInputComponent* EnhancedInputComponent, FGameplayTag ActionTag,
	ETriggerEvent Trigger, UserObject* ContextObject, CallbackFunc Func)
{
	if (!InputDataAsset || !EnhancedInputComponent) return;
	
	if (InputDataAsset->GetInputActionByTag(ActionTag))
	{
		EnhancedInputComponent->BindAction(InputDataAsset->GetInputActionByTag(ActionTag), Trigger, ContextObject, Func);
	}
}
