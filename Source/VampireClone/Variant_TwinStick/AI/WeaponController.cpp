// Fill out your copyright notice in the Description page of Project Settings.


#include "Variant_TwinStick/AI/WeaponController.h"
#include <Kismet/GameplayStatics.h>
#include "CombatMessageSubsystem.h"
#include "ProgressionMessageSubsystem.h"
#include "UpgradeDefinition.h"

// Sets default values
AWeaponController::AWeaponController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ISMComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("ISMComponent"));
	RootComponent = ISMComponent;
	ISMComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeaponController::Setup()
{
	//AActor* swarmInstance = Cast<ASwarmManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ASwarmManager::StaticClass()));
	//_SwarmManager = Cast<ASwarmManager>(swarmInstance);
	//if(!_SwarmManager) { return; }

	_hitDistanceSquared = FMath::Square(_scaleX*50.0f);
	_CombatRadio = GetWorld()->GetSubsystem<UCombatMessageSubsystem>();

	UProgressionMessageSubsystem* ProgressRadio = GetWorld()->GetSubsystem<UProgressionMessageSubsystem>();
	if (ProgressRadio) {
		ProgressRadio->OnUpgradeSelected.AddDynamic(this, &AWeaponController::UpgradeDelegate);
	}

	_deadProjectiles.Reserve(_projectileCount);
	_ProjectileDirection.Reserve(_projectileCount);
	_ProjectilePosition.Reserve(_projectileCount);
	_ProjectileVelocity.Reserve(_projectileCount);

	_timeLeft.Reserve(_projectileCount);
	_alive.Reserve(_projectileCount);

	FTransform HiddenTransform;
	HiddenTransform.SetLocation(FVector(0, 0, -999999.0f));

	TArray<FTransform> InstanceTransforms;
	InstanceTransforms.Init(HiddenTransform, _projectileCount);

	for (int i = 0; i < _projectileCount; i++)
	{
		_ProjectileDirection.Push(FVector(0, 0, 0));
		_ProjectilePosition.Push(FVector(0, 0, 0));
		_ProjectileVelocity.Push(FVector(0, 0, 0));

		_timeLeft.Push(0.0f);
		_alive.Push(false);

		_deadProjectiles.Push(i);
	}
	ISMComponent->AddInstances(InstanceTransforms, false);

	//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow, FString::Printf(TEXT("Actual Spawns: %d"), ISMComponent->GetInstanceCount()));
}

void AWeaponController::MoveProjectiles(float DeltaTime)
{

}

void AWeaponController::DrawProjectileMesh()
{
	for (int i = 0; i < _projectileCount; i++)
	{
		FTransform ProjectileTransform;
		if (_alive[i]) {
			ProjectileTransform.SetLocation(_ProjectilePosition[i]);
			ProjectileTransform.SetScale3D(FVector(_scaleX, _scaleY, _scaleZ));
		}
		else
		{
			ProjectileTransform.SetLocation(FVector(0, 0, -999999.0f));
		}
		ISMComponent->UpdateInstanceTransform(i, ProjectileTransform, true, false, false);
		//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Purple, FString::Printf(TEXT("Actual Spawns: %d"), ISMComponent->GetInstanceCount()));
	}
	ISMComponent->MarkRenderStateDirty();
	ISMComponent->UpdateBounds();
}

int AWeaponController::SpawnProjectile(FVector Location, FVector Direction)
{
	int nextDeadProjectile = GetNextProjectile();
	if (nextDeadProjectile == -1) { return nextDeadProjectile; }
	_alive[nextDeadProjectile] = true;
	_ProjectilePosition[nextDeadProjectile] = Location;
	_ProjectileDirection[nextDeadProjectile] = Direction.GetSafeNormal();
	_ProjectileVelocity[nextDeadProjectile] = _ProjectileDirection[nextDeadProjectile] * baseMoveSpeed * _baseSpeedModifier;
	_timeLeft[nextDeadProjectile] = 0.0f;
	return nextDeadProjectile;
}

bool AWeaponController::Upgrade(UUpgradeDefinition* upgrade)
{
	if (!_isEquipped) {
		_isEquipped = true;
		//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Purple, FString::Printf(TEXT("Actual Spawns: %d"), HISMComponent->GetInstanceCount()));
		return false;
	}
	return true;
}

void AWeaponController::Behaviour(float DeltaTime)
{
	_CurrentSpeed += DeltaTime;
}

void AWeaponController::UpgradeDelegate(UUpgradeDefinition* upgrade)
{
	Upgrade(upgrade);
}

void AWeaponController::KillProjectile(const int Projectile)
{
	_alive[Projectile] = false;
	_deadProjectiles.Push(Projectile);
}

void AWeaponController::CheckEnemyCollision()
{
	for (int i = 0; i < _projectileCount; i++)
	{
		if (!_alive[i]) { continue; }
		int closestEnemyIndex = _CombatRadio->GetClosestSwarm(_ProjectilePosition[i]);
		if (closestEnemyIndex == -1) { continue; }
		FVector* enemyPos = _CombatRadio->GetSwarmEnemyPosition(closestEnemyIndex);
		FVector* enemyScale = _CombatRadio->GetSwarmEnemyScale(closestEnemyIndex);
		float totalRadius = FMath::Square((enemyScale->X * 50) + (_scaleX * 50));
		float distance = FVector::DistSquared(_ProjectilePosition[i], *enemyPos);
		if (distance< totalRadius) {
			KillProjectile(i);
			_CombatRadio->SwarmTakeDamage(closestEnemyIndex, baseDamage * _baseDamageModifier);
		}
	}
}

// Called when the game starts or when spawned
void AWeaponController::BeginPlay()
{
	Super::BeginPlay();
	Setup();
}

// Called every frame
void AWeaponController::Tick(float DeltaTime)
{
	double startTime = FPlatformTime::Seconds();
	if (!_isEquipped) { return; }
	Super::Tick(DeltaTime);
	MoveProjectiles(DeltaTime);
	DrawProjectileMesh();
	CheckEnemyCollision();
	Behaviour(DeltaTime);
	double endTime = FPlatformTime::Seconds();
	if (endTime - startTime > 0.16) {
		UE_LOG(LogTemp, Error, TEXT("SPIKE CAUGHT: weapon Loop took %f ms"), (endTime - startTime) * 1000.0);
	}
}

int AWeaponController::GetNextProjectile()
{
	if (_deadProjectiles.Num() == 0) { return -1; }
	int nextDeadProjectile = _deadProjectiles.Pop();
	return nextDeadProjectile;
}

