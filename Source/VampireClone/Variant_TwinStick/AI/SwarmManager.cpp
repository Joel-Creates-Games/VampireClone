// Fill out your copyright notice in the Description page of Project Settings.


#include "SwarmManager.h"
#include <Kismet/GameplayStatics.h>
#include "CombatMessageSubsystem.h"
#include "ProgressionMessageSubsystem.h"

// Sets default values
ASwarmManager::ASwarmManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	HISMComponent = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("HISMComponent"));
	RootComponent = HISMComponent;
	HISMComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void ASwarmManager::BeginPlay()
{
	Super::BeginPlay();
	Setup();

	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Blue, TEXT("Setup Finished!"));
}

//DOD setup of enemies, cache player pawn, and initialize enemy properties
void ASwarmManager::Setup()
{
	UWorld* world = GetWorld();
	if (!ensure(world)) return;
	//APlayerController* playerController = world->GetFirstPlayerController();
	//if (!ensure(playerController)) return;
	//CachedPlayerPawn = playerController->GetPawn();
	//if (!ensure(CachedPlayerPawn)) return;
	ProgressRadio = GetWorld()->GetSubsystem<UProgressionMessageSubsystem>();
	if (ProgressRadio) {

	}
	CombatRadio = GetWorld()->GetSubsystem<UCombatMessageSubsystem>();
	if (CombatRadio) {
		CombatRadio->SetSwarmManager(this);
	}

	FTransform HiddenTransform;
	HiddenTransform.SetLocation(FVector(0, 0, -999999.0f));

	TArray<FTransform> InstanceTransforms;
	InstanceTransforms.Init(HiddenTransform, _enemyCount);

	_EnemyDirection.Reserve(_enemyCount);
	_EnemyPosition.Reserve(_enemyCount);
	_EnemyVelocity.Reserve(_enemyCount);
	_EnemyScale.Reserve(_enemyCount);
	_speed.Reserve(_enemyCount);
	_damage.Reserve(_enemyCount);
	_health.Reserve(_enemyCount);
	_isAlive.Reserve(_enemyCount);
	_deadEnemies.Reserve(_enemyCount);

	for (int i = 0; i < _enemyCount; i++)
	{
		_EnemyDirection.Push(FVector(0, 0, 0));
		_EnemyPosition.Push(FVector(0, 0, 0));
		_EnemyVelocity.Push(FVector(0, 0, 0));
		_EnemyScale.Push(FVector(_scale, _scale, _scale));
		_speed.Push(BaseSpeed);
		_damage.Push(BaseDamage);
		_health.Push(BaseHealth);
		_isAlive.Push(false);
		_deadEnemies.Push(i);
	}
	HISMComponent->AddInstances(InstanceTransforms, false);
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Purple, FString::Printf(TEXT("Actual Spawns: %d"), HISMComponent->GetInstanceCount()));
}

void ASwarmManager::ControlEnemy(float DeltaTime)
{
	FVector* playerPos = CombatRadio->GetPlayerPosition();
	const float AttackRangeSqrt = 10000.0f;
	for (int i = 0; i < _enemyCount; i++)
	{
		if (!_isAlive[i]) { continue; }
		FVector EnemyDirection = *playerPos - _EnemyPosition[i];
		EnemyDirection.Normalize();
		_EnemyDirection[i] = EnemyDirection;
		_EnemyVelocity[i] = EnemyDirection * _speed[i];
		_EnemyPosition[i] += _EnemyVelocity[i] * DeltaTime;

		if (FVector::DistSquared(_EnemyPosition[i], *playerPos) < AttackRangeSqrt) 
		{
			CombatRadio->OnPlayerHit.Broadcast(_damage[i]);
			KillEnemy(i);
		}
	}
}

void ASwarmManager::DrawEnemyMesh()
{
	for (int i = 0; i < _enemyCount; i++)
	{
		FTransform EnemyTransform;
		if (_isAlive[i]) {
			EnemyTransform.SetLocation(_EnemyPosition[i]);
		}
		else
		{
			EnemyTransform.SetLocation(FVector(0, 0, -999999.0f));
		}
		HISMComponent->UpdateInstanceTransform(i, EnemyTransform, true, false, false);
	}
	HISMComponent->MarkRenderStateDirty();
	HISMComponent->UpdateBounds();
}

int ASwarmManager::GetNextDeadEnemy() 
{
	if (_deadEnemies.Num() == 0) { 
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Purple, FString::Printf(TEXT("None Left: %d"), HISMComponent->GetInstanceCount()));
		return -1; }
	GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Purple, FString::Printf(TEXT("Some Left: %d"), HISMComponent->GetInstanceCount()));
	int nextDeadEnemy = _deadEnemies.Pop();
	return nextDeadEnemy;
}

void ASwarmManager::KillEnemy(const int Enemy)
{
	_isAlive[Enemy] = false;
	_deadEnemies.Push(Enemy);
	ProgressRadio->ExpDropsSpawnExp(&_EnemyPosition[Enemy]);
}

void ASwarmManager::TakeDamage(const int Enemy, const float Damage)
{
	if(!_isAlive[Enemy]) { return; }
	_health[Enemy] = FMath::Max(0, _health[Enemy] - Damage);
	if (_health[Enemy] == 0)
	{
		KillEnemy(Enemy);
	}
}

void ASwarmManager::TakeAOEDamage(const FVector& center, float radiusX, float radiusY, float damage)
{
	for (int i = 0; i < _enemyCount; i++)
	{
		if (!_isAlive[i]) { continue; }
		//float distance = FVector::DistSquared(_EnemyPosition[i], center);
		FVector relativeToPlayer = FVector(_EnemyPosition[i].X - center.X, _EnemyPosition[i].Y - center.Y, 0);
		float bounds = FMath::Square(_scale * 50);
		float ellipse = (FMath::Square(relativeToPlayer.X) / FMath::Square(radiusX+bounds)) +
			(FMath::Square(relativeToPlayer.Y) / FMath::Square(radiusY+bounds));
		if (ellipse >= 1) { continue; }
		TakeDamage(i, damage);
	}
}

// Called every frame
void ASwarmManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ControlEnemy(DeltaTime);
	DrawEnemyMesh();
	_spawnTimer += DeltaTime;
	_spawnSpeedUpTimer += DeltaTime;
	if (_spawnTimer >= SpawnTime) {
		SpawnEnemy(SetSpawnLocation());
		_spawnTimer = 0.0f;
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Purple, FString::Printf(TEXT("spawned")));
	}
	if (_spawnSpeedUpTimer >= SpawnSpeedup) {
		SpawnTime = FMath::Max(0.3f, SpawnTime - 0.1f);
		_spawnSpeedUpTimer = 0;
	}
}

void ASwarmManager::SpawnEnemy(const FVector& SpawnLocation)
{
	int nextDeadEnemy = GetNextDeadEnemy();
	if (nextDeadEnemy == -1) { return; }

	_isAlive[nextDeadEnemy] = true;
	_EnemyPosition[nextDeadEnemy] = SpawnLocation;
}

FVector ASwarmManager::SetSpawnLocation()
{
	FVector* playerPos = CombatRadio->GetPlayerPosition();
	float angle = FMath::RandRange(0.0f, 2.0f * PI);
	float radius = 3000.0f;
	float xOffset = FMath::Cos(angle) * radius;
	float yOffset = FMath::Sin(angle) * radius;
	FVector SpawnLocation = *playerPos + FVector(xOffset, yOffset, 0.0f);
	return SpawnLocation;
}

int ASwarmManager::GetClostestEnemyIndex(const FVector& Location)
{
	int closestEnemyIndex = -1;
	int minDistance = INT_MAX;
	for (int i = 0; i < _enemyCount; i++)
	{
		if (!_isAlive[i]) { continue; }
		int distance = FVector::DistSquared(_EnemyPosition[i], Location);
		if (distance < minDistance)
		{
			minDistance = distance;
			closestEnemyIndex = i;
		}
	}
	return closestEnemyIndex;
}

TArray<FVector>* ASwarmManager::GetEnemyPositions()
{
	return &_EnemyPosition;
}

FVector* ASwarmManager::GetEnemyPosition(const int EnemyIndex)
{
	return &_EnemyPosition[EnemyIndex];
}

FVector* ASwarmManager::GetEnemyScale(int EnemyIndex)
{
	return &_EnemyScale[EnemyIndex];
}
