// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponController.h"
#include "MagicWandController.generated.h"

UCLASS()
class VAMPIRECLONE_API AMagicWandController : public AWeaponController
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMagicWandController();


protected:
	// Called when the game starts or when spawned
	//virtual void BeginPlay() override;
	virtual void Setup() override;
	virtual void MoveProjectiles(float DeltaTime) override;
	//virtual void DrawProjectileMesh() override;
	//virtual void KillProjectile(const int Projectile) override;
	//virtual void CheckEnemyCollision() override;
	virtual void Behaviour(float DeltaTime) override;

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

	//virtual int GetNextProjectile() override;
	//virtual void SpawnProjectile(FVector Location, FVector Direction) override;
	virtual bool Upgrade(UUpgradeDefinition* upgrade) override;

private:
	bool _isEquipped = true;
};
