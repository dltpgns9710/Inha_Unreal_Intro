// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/EnemyFSM.h"

#include "Inha_Intro.h"
#include "Animation/EnemyAnimation.h"
#include "Character/Enemy.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/BaseCharacter.h"

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
	}
}

void UEnemyFSM::MoveState()
{
	FVector Destination = Target->GetActorLocation();
	FVector Dir = Destination - Owner->GetActorLocation();
	Owner->AddMovementInput(Dir.GetSafeNormal());
	
	FVector DirectionToTarget = (Target->GetActorLocation() - Owner->GetActorLocation()).GetSafeNormal();
	float DotResult = FVector::DotProduct(Owner->GetActorForwardVector(), DirectionToTarget);
	bool isInSight = DotResult >= FMath::Cos(FMath::DegreesToRadians(SightDegree/2));
	
	if (isInSight && Dir.Size() < AttackRange)
	{
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
	}
	
	CurrentTime += GetWorld()->GetDeltaSeconds();
	if (CurrentTime > AttackDelayTime)
	{
		PRINT_LOG(TEXT("Attack"));	
		CurrentTime = 0;
		CachedAnim->SetCanAttackPlay(true);
	}
	
	
	/*if (!CachedAnim->IsAttackPlaying() && Distance > AttackRange)
	{
		CurrentTime = 0;
		State = EEnemyState::Move;
		CachedAnim->SetAnimState(State);
	}*/
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

