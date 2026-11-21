// Fill out your copyright notice in the Description page of Project Settings.


#include "Variant_TwinStick/AI/BibleAttack.h"
#include "CombatMessageSubsystem.h"
#include "UpgradeDefinition.h"

ABibleAttack::ABibleAttack()
{
}

void ABibleAttack::Setup()
{
	_AttackSpeed = 3.0f;
	_projectileCount = 20;
	_baseSpeedModifier = 1.5f;
	Super::Setup();
	//_isEquipped = true;
}

void ABibleAttack::Behaviour(float DeltaTime)
{
	if (_isSpinning) {
		_currentSpinTime += DeltaTime;
		CalculateOrbitPositions(DeltaTime);
		CheckEnemyCollision();
		if (_currentSpinTime > _spinTime) {
			_isSpinning = false;
			_currentSpinTime = 0;
			StopSpinningProjectiles();
			if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("stop"));
		}
		return;
	}
	Super::Behaviour(DeltaTime);
	if (_CurrentSpeed > _AttackSpeed) {
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("attack"));
		_CurrentSpeed = 0;
		_isSpinning = true;
		SetupProjectile();
	}
}

void ABibleAttack::CalculateOrbitPositions(float DeltaTime)
{
	float SinChange = FMath::Sin(_spinSpeed * DeltaTime);
	float CosChange = FMath::Cos(_spinSpeed * DeltaTime);

	FVector* playerPos = _CombatRadio->GetPlayerPosition();
	for (size_t i = 0; i < _activeCount; i++)
	{
		//if (!_alive[i]) { continue; }
		float oldX = _ProjectilePosition[i].X;
		float oldY = _ProjectilePosition[i].Y;
		float newX = (oldX * CosChange) - (oldY * SinChange);
		float newY = (oldX * SinChange) + (oldY * CosChange);
		_ProjectilePosition[i].X = newX;
		_ProjectilePosition[i].Y = newY;
	}
}

void ABibleAttack::SetupProjectile()
{
	float angle = (2 * UE_PI) / _multiAttackMax;
	FVector* playerPos = _CombatRadio->GetPlayerPosition();
	for (size_t i = 0; i < _multiAttackMax; i++)
	{
		int pos = SpawnProjectile(FVector::Zero(), FVector::Zero());
		_ProjectilePosition[pos].X = (FMath::Cos(angle * i) * _RadiusFromPlayer);
		_ProjectilePosition[pos].Y = (FMath::Sin(angle * i) * _RadiusFromPlayer);
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, _ProjectilePosition[pos].ToString());
	}
}

void ABibleAttack::StopSpinningProjectiles()
{
	for (size_t i = 0; i < _activeCount; i++)
	{
		//if (!_alive[i]) { continue; }
		KillProjectile(i);
	}
}

void ABibleAttack::CheckEnemyCollision()
{
	FVector* playerPos = _CombatRadio->GetPlayerPosition();
	for (int i = 0; i < _activeCount; i++)
	{
		//if (!_alive[i]) { continue; }
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Purple, FString::Printf(TEXT("active: %d"), _activeCount));
		int closestEnemyIndex = _CombatRadio->GetClosestSwarm(_ProjectilePosition[i] + *playerPos);
		if (closestEnemyIndex == -1) { continue; }
		FVector* enemyPos = _CombatRadio->GetSwarmEnemyPosition(closestEnemyIndex);
		FVector* enemyScale = _CombatRadio->GetSwarmEnemyScale(closestEnemyIndex);
		float totalRadius = FMath::Square((enemyScale->X * 50) + (_scaleX * 50));
		float distance = FVector::DistSquared(_ProjectilePosition[i] + *playerPos, *enemyPos);
		if (distance < totalRadius) {
			_CombatRadio->SwarmTakeDamage(closestEnemyIndex, baseDamage * _baseDamageModifier);
		}
	}
}

void ABibleAttack::DrawProjectileMesh()
{
	FVector* playerPos = _CombatRadio->GetPlayerPosition();
	for (int i = 0; i < _activeCount; i++)
	{
		_InstanceTransforms[i].SetLocation(_ProjectilePosition[i] + *playerPos);
	}
	ISMComponent->BatchUpdateInstancesTransforms(0, _InstanceTransforms, true, false, true);
}

bool ABibleAttack::Upgrade(UUpgradeDefinition* upgrade)
{
	if (!upgrade->weaponTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Weapons.Bible")))) { return false; }
	if (!Super::Upgrade(upgrade)) { return false; }
	_multiAttackMax = FMath::Min(_multiAttackMax + upgrade->AddAttacks, _projectileCount);
	_AttackSpeed = FMath::Max(0.3f, _AttackSpeed - upgrade->AttackSpeedChange);
	_baseDamageModifier *= upgrade->DamageMultiplier;
	_spinSpeed += upgrade->SpeedMultiplier;
	_scaleX += upgrade->AreaChange;
	_scaleY += upgrade->AreaChange;
	return false;
}
