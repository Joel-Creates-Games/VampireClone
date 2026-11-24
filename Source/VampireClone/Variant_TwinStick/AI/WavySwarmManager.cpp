// Fill out your copyright notice in the Description page of Project Settings.


#include "Variant_TwinStick/AI/WavySwarmManager.h"
#include "WavySwarmManager.h"
#include "CombatMessageSubsystem.h"

void AWavySwarmManager::ControlEnemy(float DeltaTime)
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
