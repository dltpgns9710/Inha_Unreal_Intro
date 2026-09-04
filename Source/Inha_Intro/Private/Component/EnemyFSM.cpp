// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/EnemyFSM.h"

#include "AIController.h"
#include "Inha_Intro.h"
#include "NavigationSystem.h"
#include "Animation/EnemyAnimation.h"
#include "Character/Enemy.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/BaseCharacter.h"
#include "Navigation/PathFollowingComponent.h"

// Sets default values for this component's properties
UEnemyFSM::UEnemyFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UEnemyFSM::BeginPlay()
{
	Super::BeginPlay();

	// ...
	AActor* Actor = UGameplayStatics::GetActorOfClass(GetWorld(), ABaseCharacter::StaticClass());
	Target = Cast<ABaseCharacter>(Actor);
	Owner = Cast<AEnemy>(GetOwner());
	
	CachedAnim = Cast<UEnemyAnimation>(Owner->GetMesh()->GetAnimInstance());
	OwnerAiController = Cast<AAIController>(Owner->GetController());
}


// Called every frame
void UEnemyFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	switch (State) {
	case EEnemyState::Idle:
		IdleState();
		break;
	case EEnemyState::Move:
		MoveState();
		break;
	case EEnemyState::Attack:
		AttackState();
		break;
	case EEnemyState::Damage:
		DamageState();
		break;
	case EEnemyState::Die:
		DieState();
		break;
	}
}

void UEnemyFSM::IdleState()
{
	CurrentTime += GetWorld()->GetDeltaSeconds();
	if (CurrentTime > IdleDelayTime)
	{
		State = EEnemyState::Move;
		CurrentTime = 0;
		CachedAnim->SetAnimState(State);
		GetRandomPositionInNavMesh(Owner->GetActorLocation(), RandomPositionRadius, RandomPosition);
	}
}

void UEnemyFSM::MoveState()
{
	FVector Destination = Target->GetActorLocation();
	FVector Dir = Destination - Owner->GetActorLocation();
	
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	
	FPathFindingQuery Query;
	FAIMoveRequest Req;
	Req.SetAcceptanceRadius(3);
	Req.SetGoalLocation(Destination);
	// 길 찾기를 위한 쿼리 생성
	OwnerAiController->BuildPathfindingQuery(Req, Query);

	// 끊긴 길은 찾기 않도록 설정
	Query.SetAllowPartialPaths(false);
	
	FPathFindingResult PathFindingQuery = NavSystem->FindPathSync(Query);
	
	if (PathFindingQuery.Result == ENavigationQueryResult::Success)
	{
		OwnerAiController->MoveToLocation(Destination);
	}
	else
	{
		EPathFollowingRequestResult::Type Result = OwnerAiController->MoveToLocation(RandomPosition);
		if (Result == EPathFollowingRequestResult::AlreadyAtGoal)
		{
			GetRandomPositionInNavMesh(Owner->GetActorLocation(), RandomPositionRadius, RandomPosition);
		}
	}
	
	FVector DirectionToTarget = (Target->GetActorLocation() - Owner->GetActorLocation()).GetSafeNormal();
	float DotResult = FVector::DotProduct(Owner->GetActorForwardVector(), DirectionToTarget);
	bool isInSight = DotResult >= FMath::Cos(FMath::DegreesToRadians(SightDegree/2));
	
	if (isInSight && Dir.Size() < AttackRange)
	{
		OwnerAiController->StopMovement();
		State = EEnemyState::Attack;
		CachedAnim->SetAnimState(State);
		CachedAnim->SetCanAttackPlay(true);
		CurrentTime = AttackDelayTime;
	}
}

void UEnemyFSM::AttackState()
{
	FVector DirectionToTarget = (Target->GetActorLocation() - Owner->GetActorLocation()).GetSafeNormal();
	float DotResult = FVector::DotProduct(Owner->GetActorForwardVector(), DirectionToTarget);
	bool isInSight = DotResult >= FMath::Cos(FMath::DegreesToRadians(SightDegree/2));
	float Distance = FVector::Distance(Target->GetActorLocation(), Owner->GetActorLocation());
	
	if (!CachedAnim->IsAttackPlaying() && (!isInSight || Distance > AttackRange))
	{
		CurrentTime = 0;
		State = EEnemyState::Move;
		CachedAnim->SetAnimState(State);
		GetRandomPositionInNavMesh(Owner->GetActorLocation(), RandomPositionRadius, RandomPosition);
	}
	
	CurrentTime += GetWorld()->GetDeltaSeconds();
	if (CurrentTime > AttackDelayTime)
	{
		PRINT_LOG(TEXT("Attack"));	
		CurrentTime = 0;
		CachedAnim->SetCanAttackPlay(true);
	}
}

void UEnemyFSM::OnDamageProcess()
{
	--Hp;
	if (Hp > 0)
	{
		State = EEnemyState::Damage;
		
		CurrentTime = 0;
		
		int32 index = FMath::RandRange(0, 1);
		FString SectionName = FString::Printf(TEXT("Damage%d"), index);
		CachedAnim->OnDamaged(FName(*SectionName));
	}
	else
	{
		State = EEnemyState::Die;
		Owner->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CachedAnim->OnDamaged(FName(TEXT("Dead")));
	}
	CachedAnim->SetAnimState(State);
	OwnerAiController->StopMovement();
}

bool UEnemyFSM::GetRandomPositionInNavMesh(FVector Center, float Radius, FVector& Dest)
{
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	FNavLocation NavLocation;
	bool Result = NavSystem->GetRandomReachablePointInRadius(Center, Radius, NavLocation);
	Dest = NavLocation.Location;
	return Result;
}

void UEnemyFSM::DamageState()
{
	CurrentTime += GetWorld()->GetDeltaSeconds();
	if (CurrentTime > DamageDelayTime)
	{
		State = EEnemyState::Idle;
		CachedAnim->SetAnimState(State);
		CurrentTime = 0;
	}
}

void UEnemyFSM::DieState()
{
	if (!CachedAnim->IsDieDone()) return;
	
	FVector P0 = Owner->GetActorLocation();
	FVector Vt = FVector::DownVector * DieSpeed * GetWorld()->GetDeltaSeconds();
	FVector P = P0 + Vt;
	Owner->SetActorLocation(P);
	
	if (P.Z < -DestroyDepth)
	{
		Owner->Destroy();
	}
}

