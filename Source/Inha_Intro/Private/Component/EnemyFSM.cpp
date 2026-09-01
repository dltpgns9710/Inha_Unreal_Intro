// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/EnemyFSM.h"

#include "Inha_Intro.h"
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
	}
}

void UEnemyFSM::MoveState()
{
	FVector Destination = Target->GetActorLocation();
	FVector Dir = Destination - Owner->GetActorLocation();
	Owner->AddMovementInput(Dir.GetSafeNormal());
	
	if (Dir.Size() < AttackRange)
	{
		State = EEnemyState::Attack;
	}
}

void UEnemyFSM::AttackState()
{
	CurrentTime += GetWorld()->GetDeltaSeconds();
	if (CurrentTime > AttackDelayTime)
	{
		PRINT_LOG(TEXT("Attack"));	
		CurrentTime = 0;
	}
	
	float Distance = FVector::Distance(Target->GetActorLocation(), Owner->GetActorLocation());
	if (Distance > AttackRange)
	{
		CurrentTime = 0;
		State = EEnemyState::Move;
	}
}

void UEnemyFSM::OnDamageProcess()
{
	--Hp;
	if (Hp > 0)
	{
		State = EEnemyState::Damage;
	}
	else
	{
		State = EEnemyState::Die;
		Owner->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void UEnemyFSM::DamageState()
{
	CurrentTime += GetWorld()->GetDeltaSeconds();
	if (CurrentTime > DamageDelayTime)
	{
		State = EEnemyState::Idle;
		CurrentTime = 0;
	}
}

void UEnemyFSM::DieState()
{
	FVector P0 = Owner->GetActorLocation();
	FVector Vt = FVector::DownVector * DieSpeed * GetWorld()->GetDeltaSeconds();
	FVector P = P0 + Vt;
	Owner->SetActorLocation(P);
	
	if (P.Z < -DestroyDepth)
	{
		Owner->Destroy();
	}
}

