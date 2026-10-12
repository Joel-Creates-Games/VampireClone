// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SwarmManager.generated.h"

UCLASS()
class VAMPIRECLONE_API ASwarmManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASwarmManager();

	UPROPERTY(VisibleAnywhere, Category = "References")
	APawn* CachedPlayerPawn = nullptr;

	UPROPERTY(EditAnywhere, Category = "Enemy Settings")
	int BaseSpeed = 100;
	UPROPERTY(EditAnywhere, Category = "Enemy Settings")
	int BaseDamage = 10;
	UPROPERTY(EditAnywhere, Category = "Enemy Settings")
	int BaseHealth = 10;

private:
	TArray<int> _deadEnemies;
	static const int _enemyCount = 10000;
	FVector _EnemyDirection[_enemyCount];
	FVector _EnemyPosition[_enemyCount];
	FVector _EnemyVelocity[_enemyCount];
	float _speed[_enemyCount];
	int _damage[_enemyCount];
	int _health[_enemyCount];
	bool _isAlive[_enemyCount];



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void Setup();
	void ControlEnemy(float DeltaTime);

	FVector SetDirection(const FVector& PlayerPos, const int Enemy);
	FVector SetVelocity(const int Enemy);
	FVector SetPosition(const int Enemy, float DeltaTime);

	int GetNextDeadEnemy();
	
	void KillEnemy(const int Enemy);
	void TakeDamage(const int Enemy, const int Damage);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SpawnEnemy(const FVector& SpawnLocation);
};
