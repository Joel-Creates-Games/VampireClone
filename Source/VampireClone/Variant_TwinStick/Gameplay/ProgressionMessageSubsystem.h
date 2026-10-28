// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameplayTagContainer.h"
#include "ProgressionMessageSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnExperienceGain, float, GainAmount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelUp, float, Level);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpgradeSelected, UUpgradeDefinition*, Upgrade);


UCLASS()
class UProgressionMessageSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	FOnExperienceGain OnExperienceGain;

	FOnLevelUp OnLevelUp;

	FOnUpgradeSelected OnUpgradeSelected;



public:
	void ExpDropsSpawnExp(FVector* position);
	void SetExpDrops(class AExpDrops* exp);
	AExpDrops& GetExpDrops() { return *_exp; }

	//broadcasting
public:
	UFUNCTION(BlueprintCallable, Category = "Progression Subsystem")
	void BroadcastOnUpgradeSelected(UUpgradeDefinition* tag);

private:
	AExpDrops* _exp;
};
