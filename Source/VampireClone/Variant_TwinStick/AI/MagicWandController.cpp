// Fill out your copyright notice in the Description page of Project Settings.


#include "Variant_TwinStick/AI/MagicWandController.h"
#include <Kismet/GameplayStatics.h>
#include "CombatMessageSubsystem.h"
#include "ProgressionMessageSubsystem.h"
#include "UpgradeDefinition.h"

// Sets default values
AMagicWandController::AMagicWandController()
{

}

void AMagicWandController::Setup()
{
	Super::Setup();
}

bool AMagicWandController::Upgrade(UUpgradeDefinition* upgrade)
{
	if (!Super::Upgrade(upgrade)) { return false; }
	if (upgrade->weaponTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Weapons.Wand")))) 
	{
		_baseSpeedModifier *= upgrade->SpeedMultiplier;
		_baseDamageModifier *= upgrade->DamageMultiplier;
		_AttackSpeed = FMath::Max(0.3f, _AttackSpeed - upgrade->AttackSpeedChange);
	}
	return false;
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

void AMagicWandController::Behaviour(float DeltaTime)
{
	Super::Behaviour(DeltaTime);
	if (_CurrentSpeed > _AttackSpeed) {
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

