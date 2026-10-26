// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "MagicWandController.generated.h"

UCLASS()
class VAMPIRECLONE_API AMagicWandController : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMagicWandController();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UHierarchicalInstancedStaticMeshComponent* HISMComponent;

	static const int _projectileCount = 100;

	UPROPERTY(EditAnywhere, Category = "Statistics")
	float Lifetime = 10.0f;
	UPROPERTY(EditAnywhere, Category = "Statistics")
	float baseMoveSpeed = 600.0f;
	UPROPERTY(EditAnywhere, Category = "Statistics")
	float baseDamage = 1;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void Setup();
	void MoveProjectiles(float DeltaTime);
	void DrawProjectileMesh();
	void KillProjectile(const int Projectile);
	void CheckEnemyCollision();
	void MagicWand(float DeltaTime);

protected:
	//class ASwarmManager* _SwarmManager = nullptr;

	class UCombatMessageSubsystem* _CombatRadio = nullptr;

	TArray<int> _deadProjectiles;
	FVector _ProjectileDirection[_projectileCount];
	FVector _ProjectilePosition[_projectileCount];
	FVector _ProjectileVelocity[_projectileCount];

	float _timeLeft[_projectileCount];
	bool _alive[_projectileCount];

	float _baseSpeedModifier = 1.0f;
	float _baseDamageModifier = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Magic Wand")
	float wandAttackSpeed = 2.0f;
	float wandCurrentSpeed = 0.0f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	int GetNextProjectile();
	void SpawnProjectile(FVector Location, FVector Direction);
};
