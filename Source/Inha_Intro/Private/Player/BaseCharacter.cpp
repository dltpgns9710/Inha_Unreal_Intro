// Fill out your copyright notice in the Description page of Project Settings.

#include "Public/Player/BaseCharacter.h"

#include "InputActionValue.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshPath(TEXT("/Script/Engine.SkeletalMesh'/Game/Assets/ThirdPersonTemplate/Characters/Mannequins/Meshes/SKM_Quinn_Simple.SKM_Quinn_Simple'"));
	
	if (MeshPath.Succeeded())
	{
		USkeletalMeshComponent* SkeletalMeshComponent = GetMesh();
		SkeletalMeshComponent->SetSkeletalMesh(MeshPath.Object);
		SkeletalMeshComponent->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
	}
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetRelativeLocation(FVector(0, 70, 90));
	CameraBoom->TargetArmLength = 400.0f;
	
	FollowCamera = CreateDefaultSubobject<UCameraComponent>("FollowCamera");
	FollowCamera->SetupAttachment(CameraBoom);
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	float dist = FVector::Dist(GetActorLocation(), FollowCamera->GetComponentLocation());
	if (dist <= 200.0f)
	{
		GetMesh()->SetVisibility(false);
	}
	else
	{
		GetMesh()->SetVisibility(true);
	}
}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

