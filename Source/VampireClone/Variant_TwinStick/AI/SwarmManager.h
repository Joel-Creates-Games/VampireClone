// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "GameplayTagContainer.h"
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
	UInstancedStaticMeshComponent* ISMComponent;



	UPROPERTY(EditAnywhere, Category = "Enemy Settings")
	int BaseSpeed = 100;
	UPROPERTY(EditAnywhere, Category = "Enemy Settings")
	int BaseDamage = 10;
	UPROPERTY(EditAnywhere, Category = "Enemy Settings")
	int BaseHealth = 10;
	/*UPROPERTY(EditAnywhere, Category = "Enemy Settings")
	float SpawnTime = 2.0f;

	UPROPERTY(EditAnywhere, Category = "Enemy Settings")
	float SpawnSpeedup = 5.0f;*/

	UPROPERTY(EditAnywhere, Category = "Enemy Settings")
	FGameplayTag enemyTag;

	/*UPROPERTY(EditAnywhere, Category = "Enemy Settings")
	class UCurveFloat* SpawnCurve = nullptr;*/

protected:
	class UProgressionMessageSubsystem* ProgressRadio = nullptr;
	class UCombatMessageSubsystem* CombatRadio = nullptr;

private:

	TArray<uint8> _cleanupFlags;

	TArray<FTransform> _InstanceTransforms;
	TArray<FVector> _EnemyScale;
	TArray<float> _damage;
	TArray<float> _health;

	int _enemyCount = 10000;

	/// <summary>
	/// update this later for different enemies
	/// </summary>
	float _scale = 1.0f;

	float _spawnTimer = 0.0f;
	float _spawnSpeedUpTimer = 0.0f;

	int _SpawnCount = 3;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void Setup();

	//in the tick function, we will control the enemies, moving them towards the player and checking for collisions
	virtual void ControlEnemy(float DeltaTime);
	void DrawEnemyMesh();

	int GetNextDeadEnemy();
	
	void KillEnemy(const int Index);

protected:
	TArray<FVector> _EnemyPosition;
	TArray<FVector> _EnemyDirection;
	TArray<FVector> _EnemyVelocity;

	TArray<float> _speed;

	int _aliveEnemiesCount = 0;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	FVector SetSpawnLocation();
	FVector* GetEnemyPosition(const int EnemyIndex);
	FVector* GetEnemyScale(int EnemyIndex);
	int GetClosestEnemyIndex(const FVector& Location);
	TArray<FVector>* GetEnemyPositions();
	void TakeDamage(const int Index, const float Damage);
	void TakeAOEDamage(const FVector& center, float radiusX, float radiusY, float damage);
	void SpawnEnemy(const FVector& SpawnLocation);
	void SpawnEnemies(const int Count);

	UFUNCTION()
	void SpawnEnemyListener(FGameplayTag spawnTag);


/// <summary>
/// Collision logic for spatial hashing, we will check for collisions between enemies and the player or attacks
/// </summary>
private:
	TArray<int> hitZonesX;
	TArray<int> hitZonesY;
	TArray<TArray<int>> spatialGrid;
	int hitZoneDepth = 100;
	int zoneWidth = 20;
	int zoneHeight = 20;

private:
	int CalculateHitZone(const FVector& Location);
	void UpdateGrid();
	int CheckClosestGrid(const FVector& Location);
	void SetupSpatialGrid();
	void CleanupTickFrame();
	void SwapSOA(int indexA, int indexB);
};
