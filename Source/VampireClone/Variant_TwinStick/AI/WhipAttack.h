// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Variant_TwinStick/AI/WeaponController.h"
#include "WhipAttack.generated.h"

/**
 * 
 */
UCLASS()
class VAMPIRECLONE_API AWhipAttack : public AWeaponController
{
	GENERATED_BODY()

public:
	AWhipAttack();

protected:
	virtual void Setup() override;
	virtual void Behaviour(float DeltaTime) override;
	virtual void MoveProjectiles(float DeltaTime) override;
	virtual int SpawnProjectile(FVector Location, FVector Direction) override;
	virtual void CheckEnemyCollision() override;

	UFUNCTION()
	void SetGoingRight(bool isRight);
public:
	virtual bool Upgrade(UUpgradeDefinition* upgrade) override;

private:
	float _multiAttackDelay = 0.5f;
	float _currentMultiAttackSpeed = 0.0f;
	bool _multiAttacking = false;
	int _multiAttackMax = 1;
	int _currentMultiAttack = 0;

	float _hitDistanceSquaredY = 0.0f;

	TArray<FVector> _ProjectilePlayerOffset;

	float _goingRight = 1.0f;
	float _whipDirection = 1.0f;
	float _whipLower = 0.0f;
};
