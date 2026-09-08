// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyManager.generated.h"

class AEnemy;

UCLASS()
class INHA_INTRO_API AEnemyManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, Category=SpawnSetting)
	float MinTime = 1.f;
	UPROPERTY(EditAnywhere, Category=SpawnSetting)
	float MaxTime = 5.f;
	UPROPERTY(EditAnywhere, Category=SpawnSetting)
	TSubclassOf<AActor> SpawnPointClass;
	UPROPERTY(EditAnywhere, Category=SpawnSetting)
	TSubclassOf<AEnemy> EnemyFactory;
	
	UPROPERTY()
	TArray<AActor*> SpawnPoints;
	FTimerHandle SpawnTimerHandle;
	
	void CreateEnemy();
};
