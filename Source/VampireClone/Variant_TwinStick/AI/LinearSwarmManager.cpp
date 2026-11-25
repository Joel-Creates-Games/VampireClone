// Fill out your copyright notice in the Description page of Project Settings.


#include "Variant_TwinStick/AI/LinearSwarmManager.h"
#include "LinearSwarmManager.h"
#include "CombatMessageSubsystem.h"

void ALinearSwarmManager::ControlEnemy(float DeltaTime)
{
	FVector* playerPos = CombatRadio->GetPlayerPosition();

	for (int i = 0; i < _aliveEnemiesCount; i++)
	{
		FVector EnemyDirection = *playerPos - _EnemyPosition[i];
		EnemyDirection.Normalize();
		_EnemyDirection[i] = EnemyDirection;
		_EnemyVelocity[i] = EnemyDirection * _speed[i];
		_EnemyPosition[i] += _EnemyVelocity[i] * DeltaTime;
	}
}
