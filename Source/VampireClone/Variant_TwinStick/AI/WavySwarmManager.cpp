// Fill out your copyright notice in the Description page of Project Settings.


#include "Variant_TwinStick/AI/WavySwarmManager.h"
#include "WavySwarmManager.h"
#include "CombatMessageSubsystem.h"

void AWavySwarmManager::ControlEnemy(float DeltaTime)
{
	FVector* playerPos = CombatRadio->GetPlayerPosition();
	float time = GetWorld()->GetTimeSeconds();
	float SinChange = FMath::Sin(time) * waveHeight;
	for (int i = 0; i < _aliveEnemiesCount; i++)
	{
		FVector EnemyDirection = *playerPos - _EnemyPosition[i];

		EnemyDirection.Normalize();
		FVector RightDir = FVector::CrossProduct(FVector::UpVector, EnemyDirection).GetSafeNormal();
		FVector StrafeDirection = RightDir * SinChange;
		_EnemyDirection[i] = EnemyDirection;
		_EnemyVelocity[i] = EnemyDirection * _speed[i];
		_EnemyPosition[i] += (_EnemyVelocity[i] + StrafeDirection) * DeltaTime;
	}
}
