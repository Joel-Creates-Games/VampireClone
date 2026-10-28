// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "WeaponController.generated.h"

UCLASS()
class VAMPIRECLONE_API AWeaponController : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponController();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UHierarchicalInstancedStaticMeshComponent* HISMComponent;

	int _projectileCount = 100;
	bool _isEquipped = false;

	UPROPERTY(EditAnywhere, Category = "Statistics")
	float Lifetime = 10.0f;
	UPROPERTY(EditAnywhere, Category = "Statistics")
	float baseMoveSpeed = 600.0f;
	UPROPERTY(EditAnywhere, Category = "Statistics")
	float baseDamage = 1;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Setup();
	virtual void MoveProjectiles(float DeltaTime);
	virtual void DrawProjectileMesh();
	virtual void KillProjectile(const int Projectile);
	virtual void CheckEnemyCollision();
	virtual void Behaviour(float DeltaTime);
	virtual void UpgradeDelegate(UUpgradeDefinition* upgrade);

protected:
	//class ASwarmManager* _SwarmManager = nullptr;

	class UCombatMessageSubsystem* _CombatRadio = nullptr;

	TArray<int> _deadProjectiles;
	TArray<FVector> _ProjectileDirection;
	TArray<FVector> _ProjectilePosition;
	TArray<FVector> _ProjectileVelocity;

	TArray<float> _timeLeft;
	TArray<bool> _alive;

	float _hitDistanceSquared = 15625.0f;

	float _baseSpeedModifier = 1.0f;
	float _baseDamageModifier = 10.0f;
	float _baseAreaModifier = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Stats")
	float _AttackSpeed = 2.0f;
	UPROPERTY(EditAnywhere, Category = "Stats")
	float _CurrentSpeed = 0.0f;
	UPROPERTY(EditAnywhere, Category = "Stats")
	float _Area = 1.0f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual int GetNextProjectile();
	virtual void SpawnProjectile(FVector Location, FVector Direction);
	UFUNCTION()
	virtual bool Upgrade(UUpgradeDefinition* upgrade);
};
