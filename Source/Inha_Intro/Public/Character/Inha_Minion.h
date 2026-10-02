// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Inha_Minion.generated.h"

class UNavigationInvokerComponent;
struct FAIStimulus;
class USphereComponent;
class UAIPerceptionComponent;

UCLASS()
class INHA_INTRO_API AInha_Minion : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AInha_Minion();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minion AI")
	float PatrolSpeed = 150.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minion AI")
	float ChaseSpeed = 300.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minion AI")
	float PatrolRadius = 50000.f;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Minion AI")
	void SetNextPatrolLocation();
	UFUNCTION(BlueprintCallable, Category = "Minion AI")
	void Chase(APawn* Pawn);
	
	virtual void PostInitializeComponents() override;
	
	FORCEINLINE UAIPerceptionComponent* GetAiPerception() {return  AISense;}
	FORCEINLINE USphereComponent* GetCollision() {return  Collision;}
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Minion Perception", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAIPerceptionComponent> AISense;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Minion Perception", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> Collision;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Minion Perception", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UNavigationInvokerComponent> NavInvoker;
	
	UPROPERTY()
	TObjectPtr<APawn> TargetPawn;
	
	UPROPERTY()
	FVector PatrolLocation;

	UFUNCTION()
	void OnPawnDetected(APawn* Pawn);
	UFUNCTION()
	void OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
};
