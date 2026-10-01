#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Inha_CharacterStats.generated.h"

USTRUCT(BlueprintType)
struct INHA_INTRO_API FInha_CharacterStats : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float WalkSpeed = 300.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float RunSpeed = 600.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float NextLevelXp = 10.f;
};
