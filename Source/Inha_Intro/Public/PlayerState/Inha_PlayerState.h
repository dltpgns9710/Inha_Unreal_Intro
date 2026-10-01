// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Inha_PlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnXpChanged, int32, NewXp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelChanged, int32, NewLevelXp);

/**
 * 
 */
UCLASS(BlueprintType)
class INHA_INTRO_API AInha_PlayerState : public APlayerState
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, ReplicatedUsing = "OnRep_Xp", Category = "Exprience")
	int Xp = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, ReplicatedUsing = "OnRep_Level", Category = "Exprience")
	int Level = 1;
	
	UFUNCTION()
	void OnRep_Xp(int32 OldValue) const;
	UFUNCTION()
	void OnRep_Level(int32 OldValue) const;

public:
	UFUNCTION(BlueprintCallable, Category = "Exprience")
	void AddXp(int32 Value);
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
	
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnXpChanged OnXpChanged;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnLevelChanged OnLevelChanged;
};
