// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "UpgradeDefinition.generated.h"

/**
 * 
 */
UCLASS()
class UUpgradeDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level UI")
	FString UpgradeName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level UI")
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Broadcast")
	FGameplayTag weaponTag;

	UPROPERTY(EditAnywhere)
	float DamageMultiplier;

	UPROPERTY(EditAnywhere)
	float SpeedMultiplier;

	UPROPERTY(EditAnywhere)
	float AttackSpeedChange;
};
