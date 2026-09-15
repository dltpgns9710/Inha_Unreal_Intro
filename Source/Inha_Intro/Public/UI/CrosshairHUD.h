// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CrosshairHUD.generated.h"

/**
 * 
 */
UCLASS()
class INHA_INTRO_API ACrosshairHUD : public AHUD
{
	GENERATED_BODY()
public:
	virtual void DrawHUD() override;
	
private:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> Crosshair_Center;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> Crosshair_Left;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> Crosshair_Right;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> Crosshair_Top;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> Crosshair_Down;
	
};
