// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Variant_TwinStick/AI/WeaponController.h"
#include "BibleAttack.generated.h"

/**
 * 
 */
UCLASS()
class VAMPIRECLONE_API ABibleAttack : public AWeaponController
{
	GENERATED_BODY()
	
	ABibleAttack();

protected:

	virtual void Setup() override;
	virtual void Behaviour(float DeltaTime) override;

	void CalculateOrbitPositions(float DeltaTime);
	void SetupProjectile();
	void StopSpinningProjectiles();
	virtual void CheckEnemyCollision() override;
	virtual void DrawProjectileMesh() override;

public:
	virtual bool Upgrade(UUpgradeDefinition* upgrade) override;

protected:

	float _RadiusFromPlayer = 300.0f;
	float _spinSpeed = 1.0f;
	float _currentSpinTime = 0.0f;
	float _spinTime = 5.0f;
	bool _isSpinning = false;
};
