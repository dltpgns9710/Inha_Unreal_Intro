// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyFSM.generated.h"


class UEnemyAnimation;
class AEnemy;
class ABaseCharacter;

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Idle,
	Move,
	Attack,
	Damage,
	Die
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INHA_INTRO_API UEnemyFSM : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnemyFSM();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FSM")
	EEnemyState State = EEnemyState::Idle;
	
	void IdleState();
	void MoveState();
	void AttackState();
	void DamageState();
	void DieState();
		
	void OnDamageProcess();

protected:
	UPROPERTY(EditDefaultsOnly)
	float IdleDelayTime = 2.f;
	
	float CurrentTime = 0;

	UPROPERTY(VisibleAnywhere, Category="FSM")
	TObjectPtr<ABaseCharacter> Target;
	
	UPROPERTY()
	TObjectPtr<AEnemy> Owner;
	
	UPROPERTY(EditAnywhere, Category="FSM")
	float SightDegree = 90.f;
	
	UPROPERTY(EditAnywhere, Category="FSM")
	float AttackRange = 150.f;
	
	UPROPERTY(EditAnywhere, Category="FSM")
	float AttackDelayTime = 2.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="FSM")
	int32 Hp = 3;
	
	UPROPERTY(EditAnywhere, Category="FSM")
	float DamageDelayTime = 2.f;
	
	UPROPERTY(EditAnywhere, Category="FSM")
	float DieSpeed = 50.f;

	UPROPERTY(EditAnywhere, Category="FSM")
	float DestroyDepth = 200.f;
	
private:
	UPROPERTY()
	TObjectPtr<UEnemyAnimation> CachedAnim;
};
