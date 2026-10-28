// Fill out your copyright notice in the Description page of Project Settings.


#include "Variant_TwinStick/AI/GarlicAttack.h"
#include "ProgressionMessageSubsystem.h"
#include "CombatMessageSubsystem.h"
#include "UpgradeDefinition.h"

AGarlicAttack::AGarlicAttack() {

}

void  AGarlicAttack::Setup() {

	_hitDistanceSquared = 90000.0f;
	_projectileCount = 1;
	Super::Setup();
}

void AGarlicAttack::Behaviour(float DeltaTime) 
{
	Super::Behaviour(DeltaTime);
	if (_deadProjectiles.Num())
	{
		FVector* playerPos = _CombatRadio->GetPlayerPosition();
		if (playerPos == nullptr) { return; }
		SpawnProjectile(*playerPos, FVector::Zero());
	}
}

void AGarlicAttack::MoveProjectiles(float DeltaTime)
{
	for (int i = 0; i < _projectileCount; i++)
	{
		if (!_alive[i]) { continue; }
		FVector* playerPos = _CombatRadio->GetPlayerPosition();
		_ProjectilePosition[i] = *playerPos;
	}
}

void AGarlicAttack::CheckEnemyCollision()
{
	for (int i = 0; i < _projectileCount; i++)
	{
		if (!_alive[i]) { continue; }
		int closestEnemyIndex = _CombatRadio->GetClosestSwarm(_ProjectilePosition[i]);
		if (closestEnemyIndex == -1) { continue; }
		FVector* enemyPos = _CombatRadio->GetSwarmEnemyPosition(closestEnemyIndex);
		float distance = FVector::DistSquared(_ProjectilePosition[i], *enemyPos);
		if (distance < _hitDistanceSquared) {
			KillProjectile(i);
			_CombatRadio->SwarmTakeDamage(closestEnemyIndex, baseDamage * _baseDamageModifier);
		}
	}
}

bool AGarlicAttack::Upgrade(UUpgradeDefinition* upgrade)
{
	if (!Super::Upgrade(upgrade)) { return false; }
	if (upgrade->weaponTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Weapons.Garlic")))) 
	{
		_AttackSpeed = FMath::Max(0.3f, _AttackSpeed - upgrade->AttackSpeedChange);
		_baseAreaModifier *= upgrade->AreaChange;
		_baseDamageModifier *= upgrade->DamageMultiplier;
	}
	return false;
}
