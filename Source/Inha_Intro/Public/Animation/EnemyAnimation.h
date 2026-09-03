// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyAnimation.generated.h"

class AEnemy;
enum class EEnemyState : uint8;
/**
 * 
 */
UCLASS()
class INHA_INTRO_API UEnemyAnimation : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	void SetAnimState(EEnemyState State);
	void SetCanAttackPlay(bool bCanAttack);
	void OnDamaged(FName SectionName);
	bool IsDieDone() const;
	bool IsAttackPlaying() const;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = FSM)
	EEnemyState AnimState;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = FSM)
	TObjectPtr<AEnemy> CachedOwner;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = FSM)
	bool bIsAttackPlaying = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = FSM)
	bool bCanAttackPlay = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = FSM)
	bool bDieDone = false;
	
	UFUNCTION(BlueprintCallable, Category = FSMEvent)
	void OnEndAttackCallback();
	
	UFUNCTION(BlueprintImplementableEvent, Category = FSMEvent)
	void OnDamageCallback(FName SectionName);
	
	UFUNCTION(BlueprintCallable, Category = FSMEvent)
	AEnemy* GetCastedOwner();
};
