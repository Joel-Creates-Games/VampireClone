// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Variant_TwinStick/AI/WeaponController.h"
#include "DartAttack.generated.h"

/**
 * 
 */
UCLASS()
class VAMPIRECLONE_API ADartAttack : public AWeaponController
{
	GENERATED_BODY()

	ADartAttack();
	
public:
	virtual bool Upgrade(UUpgradeDefinition* upgrade) override;
protected:
	virtual void Setup() override;
	virtual void MoveProjectiles(float DeltaTime) override;
	virtual void CheckEnemyCollision() override;
	virtual void Behaviour(float DeltaTime) override;
	virtual int SpawnProjectile(FVector Location, FVector Direction) override;

	virtual void SwapSOA(int posA, int posB) override;

protected:
	TArray<TArray<int>> _hitList;

	int _pierceCount = 1;
};
