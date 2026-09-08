// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/EnemyManager.h"

#include "Character/Enemy.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemyManager::AEnemyManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AEnemyManager::BeginPlay()
{
	Super::BeginPlay();
	
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), SpawnPointClass, SpawnPoints);
	
	float CreateTime = FMath::RandRange(MinTime, MaxTime);
	GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &ThisClass::CreateEnemy, CreateTime);
}

// Called every frame
void AEnemyManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemyManager::CreateEnemy()
{
	int index = FMath::RandRange(0, SpawnPoints.Num() - 1);
	GetWorld()->SpawnActor<AEnemy>(EnemyFactory, SpawnPoints[index]->GetActorLocation(), FRotator(0));
	
	float CreateTime = FMath::RandRange(MinTime, MaxTime);
	
	GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &ThisClass::CreateEnemy, CreateTime);
}

