// Fill out your copyright notice in the Description page of Project Settings.


#include "Variant_TwinStick/AI/MagicWandController.h"
#include <Kismet/GameplayStatics.h>
#include "SwarmManager.h"

// Sets default values
AMagicWandController::AMagicWandController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	HISMComponent = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("HISMComponent"));
	RootComponent = HISMComponent;
	HISMComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void AMagicWandController::BeginPlay()
{
	Super::BeginPlay();
	Setup();
}

void AMagicWandController::Setup()
{
	AActor* swarmInstance = Cast<ASwarmManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ASwarmManager::StaticClass()));
	_SwarmManager = Cast<ASwarmManager>(swarmInstance);
	if(!_SwarmManager) { return; }
	_deadProjectiles.Reserve(_projectileCount);

	FTransform HiddenTransform;
	HiddenTransform.SetLocation(FVector(0, 0, -999999.0f));

	TArray<FTransform> InstanceTransforms;
	InstanceTransforms.Init(HiddenTransform, _projectileCount);

	for (int i = 0; i < _projectileCount; i++)
	{
		_ProjectileDirection[i] = FVector(0, 0, 0);
		_ProjectilePosition[i] = FVector(0, 0, 0);
		_ProjectileVelocity[i] = FVector(0, 0, 0);

		_timeLeft[i] = 0.0f;
		_alive[i] = false;

		_deadProjectiles.Push(i);
	}
	HISMComponent->AddInstances(InstanceTransforms, false);

	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow, FString::Printf(TEXT("Actual Spawns: %d"), HISMComponent->GetInstanceCount()));
}

void AMagicWandController::MoveProjectiles(float DeltaTime)
{
	for (int i = 0; i < _projectileCount; i++)
	{
		if (!_alive[i]) { continue; }
		_ProjectilePosition[i] += _ProjectileVelocity[i] * DeltaTime;
		_timeLeft[i] += DeltaTime;
		if (_timeLeft[i] >= Lifetime) {
			KillProjectile(i);
		}
	}
}

void AMagicWandController::DrawProjectileMesh()
{
	for (int i = 0; i < _projectileCount; i++)
	{
		FTransform ProjectileTransform;
		if (_alive[i]) {
			ProjectileTransform.SetLocation(_ProjectilePosition[i]);
		}
		else
		{
			ProjectileTransform.SetLocation(FVector(0, 0, -999999.0f));
		}
		HISMComponent->UpdateInstanceTransform(i, ProjectileTransform, true, false, false);
		//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Purple, FString::Printf(TEXT("Actual Spawns: %d"), HISMComponent->GetInstanceCount()));
	}
	HISMComponent->MarkRenderStateDirty();
	HISMComponent->UpdateBounds();
}

void AMagicWandController::SpawnProjectile(FVector Location, FVector Direction)
{
	int nextDeadProjectile = GetNextProjectile();
	if (nextDeadProjectile == -1) { return; }
	_alive[nextDeadProjectile] = true;
	_ProjectilePosition[nextDeadProjectile] = Location;
	_ProjectileDirection[nextDeadProjectile] = Direction.GetSafeNormal();
	_ProjectileVelocity[nextDeadProjectile] = _ProjectileDirection[nextDeadProjectile] * baseMoveSpeed * _baseSpeedModifier;
	_timeLeft[nextDeadProjectile] = 0.0f;
}

void AMagicWandController::KillProjectile(const int Projectile)
{
	_alive[Projectile] = false;
	_deadProjectiles.Push(Projectile);
}

void AMagicWandController::CheckEnemyCollision()
{
	for (int i = 0; i < _projectileCount; i++)
	{
		if (!_alive[i]) { continue; }
		int closestEnemyIndex = _SwarmManager->GetClostestEnemyIndex(_ProjectilePosition[i]);
		if (closestEnemyIndex == -1) { continue; }
		FVector enemyPos = _SwarmManager->GetEnemyPosition(closestEnemyIndex);
		float distance = FVector::DistSquared(_ProjectilePosition[i], enemyPos);
		if (distance < 15625.0f) {
			KillProjectile(i);
			_SwarmManager->TakeDamage(closestEnemyIndex, baseDamage * _baseDamageModifier);
		}
	}
}

// Called every frame
void AMagicWandController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MoveProjectiles(DeltaTime);
	DrawProjectileMesh();
	CheckEnemyCollision();
}

int AMagicWandController::GetNextProjectile()
{
	if (_deadProjectiles.Num() == 0) { return -1; }
	int nextDeadProjectile = _deadProjectiles.Pop();
	return nextDeadProjectile;
}

