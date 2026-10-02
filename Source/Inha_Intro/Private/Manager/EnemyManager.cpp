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
	
	if (HasAuthority() == false)
		return;
	
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
	if (SpawnPoints.Num() == 0) return;
	if (SpawnActorCount >= MaxSpawn) return;
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
	
	int index = FMath::RandRange(0, SpawnPoints.Num() - 1);
	AActor* SpawnActor =GetWorld()->SpawnActor<AActor>(EnemyFactory, SpawnPoints[index]->GetActorLocation(), FRotator(0), SpawnParams);
	if (SpawnActor)
	{
		++SpawnActorCount;
		SpawnActor->OnDestroyed.AddDynamic(this, &ThisClass::EnemyOnDestroyed);
	}
	
	float CreateTime = FMath::RandRange(MinTime, MaxTime);
	GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &ThisClass::CreateEnemy, CreateTime);
}

void AEnemyManager::EnemyOnDestroyed(AActor* DestroyedActor)
{
	--SpawnActorCount;
	SpawnActorCount = SpawnActorCount < 0 ? 0 : SpawnActorCount;
}
