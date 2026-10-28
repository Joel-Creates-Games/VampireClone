// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Variant_TwinStick/AI/WeaponController.h"
#include "GarlicAttack.generated.h"

/**
 * 
 */
UCLASS()
class VAMPIRECLONE_API AGarlicAttack : public AWeaponController
{
	GENERATED_BODY()
	

public:
	// Sets default values for this actor's properties
	AGarlicAttack();

private:

protected:
	virtual void Setup() override;
	virtual void Behaviour(float DeltaTime) override;
	virtual void MoveProjectiles(float DeltaTime) override;
	virtual void CheckEnemyCollision() override;
public:
	virtual bool Upgrade(UUpgradeDefinition* upgrade) override;
};
