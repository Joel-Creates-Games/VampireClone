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
	ISMComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("ISMComponent"));
	RootComponent = ISMComponent;
	ISMComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void ASwarmManager::BeginPlay()
{
	Super::BeginPlay();
	Setup();

	//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Blue, TEXT("Setup Finished!"));
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

	CombatRadio = GetWorld()->GetSubsystem<UCombatMessageSubsystem>();
	if (CombatRadio) {
		CombatRadio->SetSwarmManager(this);
	}

	FTransform HiddenTransform;
	HiddenTransform.SetLocation(FVector(0, 0, -999999.0f));
	_InstanceTransforms.Init(HiddenTransform, _enemyCount);

	_cleanupFlags.Init(0, _enemyCount);

	_EnemyDirection.Init(FVector(0, 0, 0), _enemyCount);
	_EnemyPosition.Init(FVector(0, 0, 0), _enemyCount);
	_EnemyVelocity.Init(FVector(0, 0, 0), _enemyCount);
	_EnemyScale.Init(FVector(_scale, _scale, _scale), _enemyCount);
	_speed.Init(BaseSpeed, _enemyCount);
	_damage.Init(BaseDamage, _enemyCount);
	_health.Init(BaseHealth, _enemyCount);
	//_deadEnemies.Reserve(_enemyCount);
	//_aliveEnemies.Reserve(_enemyCount);

	ISMComponent->AddInstances(_InstanceTransforms, false);
	ISMComponent->SetBoundsScale(10000.0f);
	//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Purple, FString::Printf(TEXT("Actual Spawns: %d"), ISMComponent->GetInstanceCount()));
}

void ASwarmManager::ControlEnemy(float DeltaTime)
{
	FVector* playerPos = CombatRadio->GetPlayerPosition();
	const float AttackRangeSqrt = 10000.0f;
	//int index = 0;
	//int size = _aliveEnemies.Num();
	for (int i = 0; i < _aliveEnemiesCount; i++)
	{
		//index = _aliveEnemies[i];
		FVector EnemyDirection = *playerPos - _EnemyPosition[i];
		EnemyDirection.Normalize();
		_EnemyDirection[i] = EnemyDirection;
		_EnemyVelocity[i] = EnemyDirection * _speed[i];
		_EnemyPosition[i] += _EnemyVelocity[i] * DeltaTime;
	}
}

void ASwarmManager::DrawEnemyMesh()
{
	//int index = 0;
	//int size = _aliveEnemies.Num();
	for (int i = 0; i < _aliveEnemiesCount; i++)
	{
		//index = _aliveEnemies[i];
		_InstanceTransforms[i].SetLocation(_EnemyPosition[i]);
	}
	ISMComponent->BatchUpdateInstancesTransforms(0, _InstanceTransforms, true, true, true);
}

int ASwarmManager::GetNextDeadEnemy() 
{
	if (_aliveEnemiesCount == _enemyCount) {
		//GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Purple, FString::Printf(TEXT("None Left: %d"), HISMComponent->GetInstanceCount()));
		return -1; }
	//GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Purple, FString::Printf(TEXT("Some Left: %d"), HISMComponent->GetInstanceCount()));
	_aliveEnemiesCount++;
	//int nextDeadEnemy = _deadEnemies.Pop();
	return _aliveEnemiesCount - 1;
}

void ASwarmManager::KillEnemy(const int Index)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(Swarm_KillEnemy);
	//_deadEnemies.Push(Enemy);
	ProgressRadio->ExpDropsSpawnExp(&_EnemyPosition[Index]);
	SwapSOA(Index, _aliveEnemiesCount-1);
	_InstanceTransforms[_aliveEnemiesCount-1].SetLocation(FVector(0, 0, -999999.0f));
	_aliveEnemiesCount--;
	//_aliveEnemies.RemoveAtSwap(Index);
	//////swap SOA data for the dead enemy with the last alive enemy
}

void ASwarmManager::TakeDamage(const int Index, const float Damage)
{
	_health[Index] = FMath::Max(0.0f, _health[Index] - Damage);
}

void ASwarmManager::TakeAOEDamage(const FVector& center, float radiusX, float radiusY, float damage)
{
	//int index = 0;
	//int size = _aliveEnemies.Num();
	for (int i = 0; i < _aliveEnemiesCount; i++)
	{
		//index = _aliveEnemies[i];
		FVector relativeToPlayer = FVector(_EnemyPosition[i].X - center.X, _EnemyPosition[i].Y - center.Y, 0);
		float bounds = _scale * 50;
		float ellipse = (FMath::Square(relativeToPlayer.X) / (radiusX + FMath::Square(bounds))) +
						(FMath::Square(relativeToPlayer.Y) / (radiusY + FMath::Square(bounds)));
		TakeDamage(i, (damage) * (ellipse < 1.0f));
	}
}

// Called every frame
void ASwarmManager::Tick(float DeltaTime)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(Swarm_Tick);
	double startTime = FPlatformTime::Seconds();
	Super::Tick(DeltaTime);
	ControlEnemy(DeltaTime);
	UpdateGrid();
	CleanupTickFrame();
	DrawEnemyMesh();
	_spawnTimer += DeltaTime;
	if (SpawnCurve && _spawnTimer >= SpawnTime)
	{
		float currentTime = GetWorld()->GetTimeSeconds()*60.0f;
		for (int i = 0; i < SpawnCurve->GetFloatValue(currentTime); i++)
		{
			SpawnEnemy(SetSpawnLocation());
		}
		_spawnTimer = 0.0f;
	}
	double endTime = FPlatformTime::Seconds();
	if (endTime - startTime > 0.16) {
		UE_LOG(LogTemp, Error, TEXT("SPIKE CAUGHT: swarm Loop took %f ms"), (endTime - startTime) * 1000.0);
	}
}

void ASwarmManager::SpawnEnemy(const FVector& SpawnLocation)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(Swarm_SpawnEnemy);
	int nextDeadEnemy = GetNextDeadEnemy();
	if (nextDeadEnemy == -1) { return; }
	_health[nextDeadEnemy] = BaseHealth;
	_EnemyPosition[nextDeadEnemy] = SpawnLocation;
}

void ASwarmManager::SpawnEnemies(const int Count)
{
	for (int i = 0; i < Count; i++)
	{
		SpawnEnemy(SetSpawnLocation());
	}
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
	TRACE_CPUPROFILER_EVENT_SCOPE(Swarm_UpdateGrid);
	for (int i = 0; i < spatialGrid.Num(); i++)
	{
		spatialGrid[i].Reset();
	}
	//int index = 0;
	//int size = _aliveEnemies.Num();
	for (int i = 0; i < _aliveEnemiesCount; i++)
	{
		//index = _aliveEnemies[i];
		int BucketIndex = CalculateHitZone(_EnemyPosition[i]);
		spatialGrid[BucketIndex].Push(i);
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
	for (int i = 0; i < _aliveEnemiesCount; i++)
	{
		bool _healthZero = _health[i] <= 0;
		bool _inAttackRange = FVector::DistSquared(_EnemyPosition[i], *playerPos) < AttackRangeSqrt;
		bool isAttacking = !_healthZero && _inAttackRange;

		_cleanupFlags[i] = _healthZero + (isAttacking * 2);
	}
	for (int i = _aliveEnemiesCount - 1; i >= 0; i--)
	{
		if (_cleanupFlags[i] == 1)
		{
			KillEnemy(i);
		}
		else if (_cleanupFlags[i] == 2)
		{
			CombatRadio->OnPlayerHit.Broadcast(_damage[i]);
			KillEnemy(i);
		}
	}
}

void ASwarmManager::SwapSOA(int indexA, int indexB)
{
	_InstanceTransforms[indexA] = _InstanceTransforms[indexB];
	_EnemyDirection[indexA] = _EnemyDirection[indexB];
	_EnemyPosition[indexA] = _EnemyPosition[indexB];
	_EnemyVelocity[indexA] = _EnemyVelocity[indexB];
	_EnemyScale[indexA] = _EnemyScale[indexB];
	_speed[indexA] = _speed[indexB];
	_damage[indexA] = _damage[indexB];
	_health[indexA] = _health[indexB];
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
	TRACE_CPUPROFILER_EVENT_SCOPE(Swarm_GetClosestEnemyIndex);
	int closestEnemyIndex = CheckClosestGrid(Location);
	if (closestEnemyIndex != -1) { return closestEnemyIndex; }
	int minDistance = INT_MAX;
	for (int i = 0; i < _aliveEnemiesCount; i++)
	{
		int distance = FVector::DistSquared(_EnemyPosition[i], Location);
		if (distance < minDistance)
		{
			minDistance = distance;
			closestEnemyIndex = i;
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
