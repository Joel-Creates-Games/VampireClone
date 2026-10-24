// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "RandomisePowerups.generated.h"

/**
 * 
 */


UCLASS(Blueprintable, BlueprintType)
class URandomisePowerups : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Upgrades")
	TArray<UUpgradeDefinition*> GetRandomUpgrades(int Count);

public:
	UPROPERTY(EditAnywhere, Category = "Upgrades")
	TArray<UUpgradeDefinition*> AllUpgrades;
	
};
