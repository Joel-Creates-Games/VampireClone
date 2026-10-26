// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "SwarmManager.generated.h"

UCLASS()
class VAMPIRECLONE_API ASwarmManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASwarmManager();

	//UPROPERTY(VisibleAnywhere, Category = "References")
	//APawn* CachedPlayerPawn = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UHierarchicalInstancedStaticMeshComponent* HISMComponent;



	UPROPERTY(EditAnywhere, Category = "Enemy Settings")
	int BaseSpeed = 100;
	UPROPERTY(EditAnywhere, Category = "Enemy Settings")
	int BaseDamage = 10;
	UPROPERTY(EditAnywhere, Category = "Enemy Settings")
	int BaseHealth = 10;
	UPROPERTY(EditAnywhere, Category = "Enemy Settings")
	float SpawnTime = 2.0f;

	float SpawnSpeedup = 15.0f;

private:
	class UProgressionMessageSubsystem* ProgressRadio = nullptr;
	class UCombatMessageSubsystem* CombatRadio = nullptr;

	TArray<int> _deadEnemies;
	static const int _enemyCount = 10;
	FVector _EnemyDirection[_enemyCount];
	FVector _EnemyPosition[_enemyCount];
	FVector _EnemyVelocity[_enemyCount];
	float _speed[_enemyCount];
	float _damage[_enemyCount];
	float _health[_enemyCount];
	bool _isAlive[_enemyCount];

	float _spawnTimer = 0.0f;
	float _spawnSpeedUpTimer = 0.0f;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void Setup();

	//in the tick function, we will control the enemies, moving them towards the player and checking for collisions
	void ControlEnemy(float DeltaTime);
	void DrawEnemyMesh();

	int GetNextDeadEnemy();
	
	void KillEnemy(const int Enemy);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	FVector SetSpawnLocation();
	FVector* GetEnemyPosition(const int EnemyIndex);
	int GetClostestEnemyIndex(const FVector& Location);
	void TakeDamage(const int Enemy, const float Damage);
	void SpawnEnemy(const FVector& SpawnLocation);
};
