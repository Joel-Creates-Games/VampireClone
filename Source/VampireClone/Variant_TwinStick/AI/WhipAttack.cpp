// Fill out your copyright notice in the Description page of Project Settings.


#include "Variant_TwinStick/AI/WhipAttack.h"
#include "CombatMessageSubsystem.h"
#include "UpgradeDefinition.h"


AWhipAttack::AWhipAttack()
{
}

void  AWhipAttack::Setup()
{
	_projectileCount = 10;
	_scaleX = 4.0f;
	_scaleY = 2.0f;
	_scaleZ = 1.0f;
	Super::Setup();
	if (_CombatRadio)
	{
		_CombatRadio->OnPlayerMoveRight.AddDynamic(this, &AWhipAttack::SetGoingRight);
	}
	_ProjectilePlayerOffset.Reserve(_projectileCount);
	for (int i = 0; i < _projectileCount; i++)
	{
		_ProjectilePlayerOffset.Push(FVector(0.0f, 0.0f, 0.0f));
	}
	_hitDistanceSquaredY = FMath::Square(_scaleY * 50);
}

void AWhipAttack::Behaviour(float DeltaTime)
{
	if (_multiAttacking) 
	{
		_currentMultiAttackSpeed += DeltaTime;
		if (_currentMultiAttackSpeed < _multiAttackDelay) { return; }
		FVector* playerPos = _CombatRadio->GetPlayerPosition();
		FVector spawnPos = FVector(playerPos->X + (((_scaleX * 100) * _goingRight)*_whipDirection), playerPos->Y - _whipLower, playerPos->Z);
		_currentMultiAttack++;
		_whipDirection *= -1.0f;
		_whipLower -= 50.0f;
		SpawnProjectile(spawnPos, FVector::Zero());
		if (_currentMultiAttack >= _multiAttackMax) {
			_currentMultiAttack = 0;
			_multiAttacking = false;
			_whipDirection = 1.0f;
			_whipLower = 0.0f;
		}
		return;
	}
	Super::Behaviour(DeltaTime);
	if (_CurrentSpeed > _AttackSpeed) 
	{
		_multiAttacking = true;
		_currentMultiAttackSpeed = _multiAttackDelay;
	}
}

void AWhipAttack::MoveProjectiles(float DeltaTime)
{
	FVector* playerPos = _CombatRadio->GetPlayerPosition();
	for (int i = 0; i < _projectileCount; i++)
	{
		if (!_alive[i]) { continue; }
		_ProjectilePosition[i] = FVector(playerPos->X + _ProjectilePlayerOffset[i].X, playerPos->Y + _ProjectilePlayerOffset[i].Y, playerPos->Z + _ProjectilePlayerOffset[i].Z);
	}
}

int AWhipAttack::SpawnProjectile(FVector Location, FVector Direction)
{
	int nextDeadProjectile = Super::SpawnProjectile(Location, Direction);
	if (nextDeadProjectile == -1) { return nextDeadProjectile; }
	FVector* playerPos = _CombatRadio->GetPlayerPosition();
	FVector offset = FVector(Location.X - playerPos->X, Location.Y - playerPos->Y, Location.Z - playerPos->Z);
	_ProjectilePlayerOffset[nextDeadProjectile] = offset;
	return nextDeadProjectile;
}

void AWhipAttack::SetGoingRight(bool isRight)
{
	if (isRight) {
		_goingRight = isRight;
	}
	_goingRight = -1.0f;
}

void AWhipAttack::CheckEnemyCollision()
{
	for (int i = 0; i < _projectileCount; i++)
	{
		_CombatRadio->SwarmTakeAOEDamage(_ProjectilePosition[i], _hitDistanceSquared, _hitDistanceSquaredY, baseDamage * _baseDamageModifier);
	}
}

bool AWhipAttack::Upgrade(UUpgradeDefinition* upgrade)
{
	if (!upgrade->weaponTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Weapons.Whip")))) { return false; }
	if (!Super::Upgrade(upgrade)) { return false; }
	_multiAttackMax += upgrade->AddAttacks;
	_multiAttackDelay = FMath::Max(0.1f, _multiAttackDelay - upgrade->AttackSpeedChange);
	_AttackSpeed = FMath::Max(0.3f, _AttackSpeed - upgrade->AttackSpeedChange);
	_baseDamageModifier *= upgrade->DamageMultiplier;
	return false;
}
