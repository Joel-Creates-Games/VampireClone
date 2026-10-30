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
	void SetGoingRight(bool isRight);

private:
	float _multiAttackDelay = 0.5f;
	float _currentMultiAttackSpeed = 0.0f;
	bool _multiAttacking = false;
	int _multiAttackMax = 1;
	int _currentMultiAttack = 0;

	TArray<FVector> _ProjectilePlayerOffset;

	float _goingRight = 1.0f;
};
