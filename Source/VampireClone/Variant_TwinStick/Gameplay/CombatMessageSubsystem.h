// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CombatMessageSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerHitSigniture, float, DamageAmount);

UCLASS()
class UCombatMessageSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FOnPlayerHitSigniture OnPlayerHit;

public:
	FVector* GetSwarmEnemyPosition(int pos);
	//TArray<FVector>* GetSwarmEnemyPositionAll();
	void SetSwarmManager(class ASwarmManager* swarm);
	void SetTwinStickCharacter(class ATwinStickCharacter* player);
	int GetClosestSwarm(FVector location);
	void SwarmTakeDamage(int index, float damage);
	void SwarmTakeAOEDamage(const FVector& center, float radius, float damage);
	FVector* GetPlayerPosition();
private:
	ASwarmManager* _swarm = nullptr;
	ATwinStickCharacter* _player = nullptr;
};
