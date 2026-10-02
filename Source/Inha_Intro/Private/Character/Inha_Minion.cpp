// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Inha_Minion.h"

#include "AIController.h"
#include "NavigationInvokerComponent.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Player/BaseCharacter.h"

// Sets default values
AInha_Minion::AInha_Minion()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AAIController::StaticClass();
	
	// AI Perception
	AISense = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AISense"));

	// Sight Config (시각)
	UAISenseConfig_Sight* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->SightRadius = 1500.f;
	SightConfig->LoseSightRadius = 1600.f;
	SightConfig->PeripheralVisionAngleDegrees = 45.f;
	SightConfig->SetMaxAge(5.0f); // 5초정도 기억하기
	AISense->ConfigureSense(*SightConfig);	// 시각 등록

	// Hearing Config (청각)
	UAISenseConfig_Hearing* HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
	HearingConfig->HearingRange = 800.f;
	AISense->ConfigureSense(*HearingConfig);	// 청각 등록

	// 메인 감각 설정
	AISense->SetDominantSense(SightConfig->GetSenseImplementation());
	
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->SetSphereRadius(100.f);
	Collision->SetupAttachment(RootComponent);
	
	GetCapsuleComponent()->InitCapsuleSize(60.f, 96.f);
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);
	
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -91.f));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MinionMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Assets/KayKit/Skeletons/skeleton_minion.skeleton_minion'"));
	if (MinionMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(MinionMesh.Object);
	}
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = 200.0f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	
	NavInvoker = CreateDefaultSubobject<UNavigationInvokerComponent>(TEXT("NavMeshInvoker"));
	NavInvoker->SetGenerationRadii(500, 800);
}

// Called when the game starts or when spawned
void AInha_Minion::BeginPlay()
{
	Super::BeginPlay();
	SetNextPatrolLocation();
}

// Called every frame
void AInha_Minion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetLocalRole() != ROLE_Authority) return;
	
	if (GetCharacterMovement()->MaxWalkSpeed == ChaseSpeed)
	{
		if (TargetPawn && (TargetPawn->GetActorLocation() - GetActorLocation()).Size() < 100.f) SetNextPatrolLocation();
		else return;
	}
	
	if ((GetActorLocation() - PatrolLocation).Size() < 500.f) SetNextPatrolLocation();
}

// Called to bind functionality to input
void AInha_Minion::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AInha_Minion::SetNextPatrolLocation()
{
	if (GetLocalRole() != ROLE_Authority) return;
	
	GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;
	
	const auto FoundNewLocation = UNavigationSystemV1::K2_GetRandomReachablePointInRadius(this, GetActorLocation(), PatrolLocation, PatrolRadius);
	if (FoundNewLocation)
	{
		TargetPawn = nullptr;
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), PatrolLocation);
	}
}

void AInha_Minion::Chase(APawn* Pawn)
{
	if (GetLocalRole() != ROLE_Authority) return;
	
	GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
	UAIBlueprintHelperLibrary::SimpleMoveToActor(GetController(), Pawn);
	TargetPawn = Pawn;
	DrawDebugSphere(GetWorld(), Pawn->GetActorLocation(), 25.f, 12, FColor::Red, true, 10.f, 0, 2.f);
}

void AInha_Minion::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (GetLocalRole() != ROLE_Authority) return;
	
	if (AISense)
	{
		AISense->OnTargetPerceptionUpdated.AddDynamic(this, &AInha_Minion::OnTargetPerceptionUpdated);
	}
}

void AInha_Minion::OnPawnDetected(APawn* Pawn)
{
	if (!Pawn->IsA<ABaseCharacter>()) return;
	
	if (GetCharacterMovement()->MaxWalkSpeed != ChaseSpeed)
	{
		Chase(Pawn); 
	}
}

void AInha_Minion::OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!OtherActor ->IsA<ABaseCharacter>()) return;
	
	
}

void AInha_Minion::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed() == false)
		return;

	FAISenseID SightID = UAISense::GetSenseID(UAISense_Sight::StaticClass());
	FAISenseID HearingID = UAISense::GetSenseID(UAISense_Hearing::StaticClass());

	// 감지된 타입에 따라서 네트워크책의 함수를 호출해준다.
	if (Stimulus.Type == SightID)
	{
		// 시각 감지
		if (APawn* Pawn = Cast<APawn>(Actor))
		{
			OnPawnDetected(Pawn); // 책 코드 호출
		}
	}
}

