// Fill out your copyright notice in the Description page of Project Settings.


#include "Variant_TwinStick/AI/SwarmManager.h"

// Sets default values
ASwarmManager::ASwarmManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASwarmManager::BeginPlay()
{
	Super::BeginPlay();
	Setup();
}

//DOD setup of enemies, cache player pawn, and initialize enemy properties
void ASwarmManager::Setup()
{
	UWorld* world = GetWorld();
	if (!ensure(world)) return;
	APlayerController* playerController = world->GetFirstPlayerController();
	if (!ensure(playerController)) return;
	CachedPlayerPawn = playerController->GetPawn();
	if (!ensure(CachedPlayerPawn)) return;

	_deadEnemies.Reserve(_enemyCount);

	for (int i = 0; i < _enemyCount; i++)
	{
		_EnemyDirection[i] = FVector(0, 0, 0);
		_EnemyPosition[i] = FVector(0, 0, 0);
		_EnemyVelocity[i] = FVector(0, 0, 0);
		_speed[i] = BaseSpeed;
		_damage[i] = BaseDamage;
		_health[i] = BaseHealth;
		_isAlive[i] = false;
		_deadEnemies.Push(i);
	}
}

void ASwarmManager::ControlEnemy(float DeltaTime)
{
	FVector playerPos = CachedPlayerPawn->GetActorLocation();
	for (int i = 0; i < _enemyCount; i++)
	{
		if (!_isAlive[i]) { continue; }
		_EnemyDirection[i] = SetDirection(playerPos, i);
		_EnemyVelocity[i] = SetVelocity(i);
		_EnemyPosition[i] += SetPosition(i, DeltaTime);
		FHitResult hitResult;
		SetActorLocation(_EnemyPosition[i], true, &hitResult);
		if(hitResult.bBlockingHit)
		{
			if (FVector::Dist(_EnemyPosition[i], playerPos) > 2.0f) { continue; }
			if(hitResult.GetActor() == CachedPlayerPawn)
			{
				KillEnemy(i);
			}
		}
	}
}

FVector ASwarmManager::SetDirection(const FVector& PlayerPos, const int Enemy) 
{
	FVector direction = PlayerPos - _EnemyPosition[Enemy];
	direction.Normalize();
	return direction;
}

FVector ASwarmManager::SetVelocity(const int Enemy) 
{
	FVector velocity = _EnemyDirection[Enemy] * _speed[Enemy];
	return velocity;
}

FVector ASwarmManager::SetPosition(const int Enemy, float DeltaTime) {
	return _EnemyVelocity[Enemy] * DeltaTime;
}

int ASwarmManager::GetNextDeadEnemy() 
{
	if (_deadEnemies.Num() == 0) { return -1; }
	int nextDeadEnemy = _deadEnemies.Pop();
	return nextDeadEnemy;
}

void ASwarmManager::KillEnemy(const int Enemy)
{
	_isAlive[Enemy] = false;
	_deadEnemies.Push(Enemy);
}

void ASwarmManager::TakeDamage(const int Enemy, const int Damage)
{
	_health[Enemy] -= Damage;
	if (_health[Enemy] <= 0)
	{
		_health[Enemy] = 0;
		KillEnemy(Enemy);
	}
}

// Called every frame
void ASwarmManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ControlEnemy(DeltaTime);
}

void ASwarmManager::SpawnEnemy(const FVector& SpawnLocation)
{
	int nextDeadEnemy = GetNextDeadEnemy();
	if (nextDeadEnemy == -1) { return; }

	_isAlive[nextDeadEnemy] = true;
	_EnemyPosition[nextDeadEnemy] = SpawnLocation;
}

