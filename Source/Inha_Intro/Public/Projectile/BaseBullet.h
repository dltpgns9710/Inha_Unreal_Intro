// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseBullet.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class INHA_INTRO_API ABaseBullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseBullet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	void Die();
	
	UPROPERTY(EditDefaultsOnly, Category="Movement", meta=(AllowPrivateAccess=true))
	float LifeSpan = 2.0;
public:
	UPROPERTY(VisibleAnywhere, Category="Movement")
	TObjectPtr<UProjectileMovementComponent> MovementComponent;
	UPROPERTY(VisibleAnywhere, Category="Collision")
	TObjectPtr<USphereComponent> SphereComponent;
	UPROPERTY(VisibleAnywhere, Category="BodyMesh")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent; 
};
