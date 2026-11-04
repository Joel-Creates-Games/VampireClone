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
	SetupSpatialGrid();
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
	_deadEnemies.Reserve(_enemyCount);
	_aliveEnemies.Reserve(_enemyCount);

	for (int i = 0; i < _enemyCount; i++)
	{
		_EnemyDirection.Push(FVector(0, 0, 0));
		_EnemyPosition.Push(FVector(0, 0, 0));
		_EnemyVelocity.Push(FVector(0, 0, 0));
		_EnemyScale.Push(FVector(_scale, _scale, _scale));
		_speed.Push(BaseSpeed);
		_damage.Push(BaseDamage);
		_health.Push(BaseHealth);
		_deadEnemies.Push(i);
	}
	HISMComponent->AddInstances(InstanceTransforms, false);
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Purple, FString::Printf(TEXT("Actual Spawns: %d"), HISMComponent->GetInstanceCount()));
}

void ASwarmManager::ControlEnemy(float DeltaTime)
{
	FVector* playerPos = CombatRadio->GetPlayerPosition();
	const float AttackRangeSqrt = 10000.0f;
	int index = 0;
	int size = _aliveEnemies.Num();
	for (int i = 0; i < size; i++)
	{
		index = _aliveEnemies[i];
		FVector EnemyDirection = *playerPos - _EnemyPosition[index];
		EnemyDirection.Normalize();
		_EnemyDirection[index] = EnemyDirection;
		_EnemyVelocity[index] = EnemyDirection * _speed[index];
		_EnemyPosition[index] += _EnemyVelocity[index] * DeltaTime;
	}
}

void ASwarmManager::DrawEnemyMesh()
{
	int index = 0;
	int size = _aliveEnemies.Num();
	for (int i = 0; i < size; i++)
	{
		index = _aliveEnemies[i];
		FTransform EnemyTransform;
		EnemyTransform.SetLocation(_EnemyPosition[index]);
		HISMComponent->UpdateInstanceTransform(index, EnemyTransform, true, false, false);
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

void ASwarmManager::KillEnemy(const int Enemy, const int Index)
{
	_deadEnemies.Push(Enemy);
	_aliveEnemies.RemoveAtSwap(Index);
	ProgressRadio->ExpDropsSpawnExp(&_EnemyPosition[Enemy]);
	FTransform EnemyTransform;
	EnemyTransform.SetLocation(FVector(0, 0, -999999.0f));
	HISMComponent->UpdateInstanceTransform(Enemy, EnemyTransform, true, false, false);
}

void ASwarmManager::TakeDamage(const int Enemy, const float Damage)
{
	_health[Enemy] = FMath::Max(0.0f, _health[Enemy] - Damage);
}

void ASwarmManager::TakeAOEDamage(const FVector& center, float radiusX, float radiusY, float damage)
{
	int index = 0;
	int size = _aliveEnemies.Num();
	for (int i = 0; i < size; i++)
	{
		index = _aliveEnemies[i];
		//float distance = FVector::DistSquared(_EnemyPosition[index], center);
		FVector relativeToPlayer = FVector(_EnemyPosition[index].X - center.X, _EnemyPosition[index].Y - center.Y, 0);
		float bounds = _scale * 50;
		float ellipse = (FMath::Square(relativeToPlayer.X) / (radiusX + FMath::Square(bounds))) +
						(FMath::Square(relativeToPlayer.Y) / (radiusY + FMath::Square(bounds)));
		TakeDamage(index, (damage) * (ellipse < 1.0f));
	}
}

// Called every frame
void ASwarmManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ControlEnemy(DeltaTime);
	UpdateGrid();
	CleanupTickFrame();
	DrawEnemyMesh();
	_spawnTimer += DeltaTime;
	_spawnSpeedUpTimer += DeltaTime;
	if (_spawnTimer >= SpawnTime) {
		for (int i = 0; i < _SpawnCount; i++)
		{
			SpawnEnemy(SetSpawnLocation());
		}
		_spawnTimer = 0.0f;
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Purple, FString::Printf(TEXT("spawned")));
	}
	if (_spawnSpeedUpTimer >= SpawnSpeedup) {
		SpawnTime = FMath::Max(0.01f, SpawnTime - 0.1f);
		_spawnSpeedUpTimer = 0;
	}
}

void ASwarmManager::SpawnEnemy(const FVector& SpawnLocation)
{
	int nextDeadEnemy = GetNextDeadEnemy();
	if (nextDeadEnemy == -1) { return; }
	_health[nextDeadEnemy] = BaseHealth;
	_aliveEnemies.Push(nextDeadEnemy);
	_EnemyPosition[nextDeadEnemy] = SpawnLocation;
}

int ASwarmManager::CalculateHitZone(const FVector& Location)
{
	int gridX = Location.X / zoneWidth;
	int gridY = Location.Y / zoneHeight;

	int WrappedX = FMath::Abs(gridX) % zoneWidth;
	int WrappedY = FMath::Abs(gridY) % zoneHeight;

	int BucketIndex = WrappedX + (WrappedY * hitZoneDepth);
	return BucketIndex;
}

void ASwarmManager::UpdateGrid()
{
	for (int i = 0; i < spatialGrid.Num(); i++)
	{
		spatialGrid[i].Reset();
	}
	int index = 0;
	int size = _aliveEnemies.Num();
	for (int i = 0; i < size; i++)
	{
		index = _aliveEnemies[i];
		int BucketIndex = CalculateHitZone(_EnemyPosition[index]);
		spatialGrid[BucketIndex].Push(index);
	}
}

void ASwarmManager::SetupSpatialGrid()
{
	spatialGrid.Reserve(hitZoneDepth * hitZoneDepth);
	for (int i = 0; i < hitZoneDepth * hitZoneDepth; i++)
	{
		spatialGrid.Push(TArray<int>());
		spatialGrid[i].Reserve(50);
	}
}

void ASwarmManager::CleanupTickFrame()
{
	FVector* playerPos = CombatRadio->GetPlayerPosition();
	const float AttackRangeSqrt = 10000.0f;
	int size = _aliveEnemies.Num();
	int Enemy = 0;
	for (int i = 0; i < size;)
	{
		Enemy = _aliveEnemies[i];
		if (_health[Enemy] == 0)
		{
			KillEnemy(Enemy, i);
			size = _aliveEnemies.Num();
		}
		else if (FVector::DistSquared(_EnemyPosition[Enemy], *playerPos) < AttackRangeSqrt)
		{
			CombatRadio->OnPlayerHit.Broadcast(_damage[Enemy]);
			KillEnemy(Enemy, i);
			size = _aliveEnemies.Num();
		}
		else
		{
			i++;
		}
	}
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

int ASwarmManager::GetClosestEnemyIndex(const FVector& Location)
{
	int closestEnemyIndex = CheckClosestGrid(Location);
	if (closestEnemyIndex != -1) { return closestEnemyIndex; }
	int minDistance = INT_MAX;
	int index = 0;
	int size = _aliveEnemies.Num();
	for (int i = 0; i < size; i++)
	{
		index = _aliveEnemies[i];
		int distance = FVector::DistSquared(_EnemyPosition[index], Location);
		if (distance < minDistance)
		{
			minDistance = distance;
			closestEnemyIndex = index;
		}
	}
	return closestEnemyIndex;
}

int ASwarmManager::CheckClosestGrid(const FVector& Location)
{
	int index = CalculateHitZone(Location);
	if (spatialGrid[index].Num() == 0) { return -1; }
	int closestEnemyIndex = -1;
	int minDistance = INT_MAX;
	for (int i = -1; i <= 1; i++)
	{
		for (int j = -1; j <= 1; j++)
		{
			int pos = (index + i) + (j * hitZoneDepth);
			if (pos < 0 || pos >= spatialGrid.Num()) { continue; }	
			for (int k = 0; k < spatialGrid[pos].Num(); k++)
			{
				int enemyIndex = spatialGrid[pos][k];
				int distance = FVector::DistSquared(_EnemyPosition[enemyIndex], Location);
				if (distance < minDistance)
				{
					minDistance = distance;
					closestEnemyIndex = enemyIndex;
				}
			}
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
