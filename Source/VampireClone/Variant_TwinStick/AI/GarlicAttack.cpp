// Fill out your copyright notice in the Description page of Project Settings.


#include "Variant_TwinStick/AI/GarlicAttack.h"
#include "ProgressionMessageSubsystem.h"
#include "CombatMessageSubsystem.h"
#include "UpgradeDefinition.h"

AGarlicAttack::AGarlicAttack() {

}

void  AGarlicAttack::Setup() {
	_projectileCount = 1;
	_scaleX = 4.0f;
	_scaleY = 4.0f;
	_scaleZ = 1.0f;
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
		return;
	}
	if (_CurrentSpeed > _AttackSpeed) 
	{
		_CurrentSpeed = 0;
		CheckEnemyCollision();
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
		_CombatRadio->SwarmTakeAOEDamage(_ProjectilePosition[i], _hitDistanceSquared, baseDamage * _baseDamageModifier);
	}
}

bool AGarlicAttack::Upgrade(UUpgradeDefinition* upgrade)
{
	if (!upgrade->weaponTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Weapons.Garlic")))) { return false; }
	if (!Super::Upgrade(upgrade)) { return false; }
	_AttackSpeed = FMath::Max(0.3f, _AttackSpeed - upgrade->AttackSpeedChange);
	_scaleX *= upgrade->AreaChange;
	_hitDistanceSquared = FMath::Square(_scaleX * 50.0f);
	_baseDamageModifier *= upgrade->DamageMultiplier;
	return false;
}
