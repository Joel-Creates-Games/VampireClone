// Fill out your copyright notice in the Description page of Project Settings.


#include "Variant_TwinStick/AI/DartAttack.h"
#include "CombatMessageSubsystem.h"
#include "UpgradeDefinition.h"

ADartAttack::ADartAttack()
{
}


void ADartAttack::Setup()
{
	_projectileCount = 20;
	Lifetime = 5.0f;
	_AttackSpeed = 5.0f;
	Super::Setup();
	_hitList.Init(TArray<int>(), _projectileCount);
	//_isEquipped = true;
}

void ADartAttack::MoveProjectiles(float DeltaTime)
{
	Super::MoveProjectiles(DeltaTime);
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Purple, FString::Printf(TEXT("active: %d"), _activeCount));
	for (size_t i = 0; i < _activeCount; i++)
	{
		//if (!_alive[i]) { continue; }
		_ProjectilePosition[i] += _ProjectileVelocity[i] * DeltaTime;
		_timeLeft[i] += DeltaTime;
		if (_timeLeft[i] > Lifetime)
		{
			KillProjectile(i);
		}
	}
}

void ADartAttack::CheckEnemyCollision()
{
	for (int i = 0; i < _activeCount; i++)
	{
		//if (!_alive[i]) { continue; }
		int closestEnemyIndex = _CombatRadio->GetClosestSwarm(_ProjectilePosition[i]);
		if (closestEnemyIndex == -1) { continue; }
		if (_hitList[i].Contains(closestEnemyIndex)) { continue; }
		FVector* enemyPos = _CombatRadio->GetSwarmEnemyPosition(closestEnemyIndex);
		FVector* enemyScale = _CombatRadio->GetSwarmEnemyScale(closestEnemyIndex);
		float totalRadius = FMath::Square((enemyScale->X * 50) + (_scaleX * 50));
		float distance = FVector::DistSquared(_ProjectilePosition[i], *enemyPos);
		if (distance < totalRadius)
		{
			_hitList[i].Push(closestEnemyIndex);
			_CombatRadio->SwarmTakeDamage(closestEnemyIndex, baseDamage * _baseDamageModifier);
			if (_hitList[i].Num() >= _pierceCount)
			{
				KillProjectile(i);
			}
		}
	}
}

void ADartAttack::Behaviour(float DeltaTime)
{
	Super::Behaviour(DeltaTime);
	if (_CurrentSpeed > _AttackSpeed / _multiAttackMax)
	{
		FVector* playPos = _CombatRadio->GetPlayerPosition();
		if (!playPos) { return; }
		_CurrentSpeed = 0;
		int closest = _CombatRadio->GetClosestSwarm(*playPos);
		if (closest == -1) { return; }
		FVector* closestEnemyLocation = _CombatRadio->GetSwarmEnemyPosition(closest);
		FVector direction = *closestEnemyLocation - *playPos;
		direction.Normalize();
		SpawnProjectile(*playPos, direction);
	}
}

int ADartAttack::SpawnProjectile(FVector Location, FVector Direction)
{
	int nextDeadProjectile = Super::SpawnProjectile(Location, Direction);
	if (nextDeadProjectile == -1) {
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No dead projectile available to spawn!"));
		return -1;
	}
	_hitList[nextDeadProjectile].Empty();
	_hitList[nextDeadProjectile].Reserve(_pierceCount);
	return nextDeadProjectile;
}

void ADartAttack::SwapSOA(int posA, int posB)
{
	Super::SwapSOA(posA, posB);
	_hitList[posA] = _hitList[posB];
}

bool ADartAttack::Upgrade(UUpgradeDefinition* upgrade)
{
	if (!upgrade->weaponTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Weapons.Dart")))) { return false; }
	if (!Super::Upgrade(upgrade)) { return false; }
	_multiAttackMax += upgrade->AddAttacks;
	_AttackSpeed = FMath::Max(1.0f, _AttackSpeed - upgrade->AttackSpeedChange);
	_baseDamageModifier *= upgrade->DamageMultiplier;
	_pierceCount += upgrade->AddAttacks;
	return false;
}
