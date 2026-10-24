// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "UpgradeDefinition.generated.h"

/**
 * 
 */
UCLASS()
class UUpgradeDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FString UpgradeName;

	UPROPERTY(EditAnywhere)
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere)
	float DamageMultiplier;

	UPROPERTY(EditAnywhere)
	float SpeedMultiplier;
};
