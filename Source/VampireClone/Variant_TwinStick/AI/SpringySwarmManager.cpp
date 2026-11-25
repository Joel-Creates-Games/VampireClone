// Fill out your copyright notice in the Description page of Project Settings.


#include "Variant_TwinStick/AI/SpringySwarmManager.h"
#include "SpringySwarmManager.h"
#include "CombatMessageSubsystem.h"

void ASpringySwarmManager::ControlEnemy(float DeltaTime)
{
	FVector* playerPos = CombatRadio->GetPlayerPosition();
	const float DampingFactor = FMath::Max(0.0f, 1.0f - Damping * DeltaTime);
	const float Time = GetWorld()->GetTimeSeconds();

	for (int i = 0; i < _aliveEnemiesCount; i++)
	{
		FVector EnemyDirection = *playerPos - _EnemyPosition[i];
		EnemyDirection.Z = 0.0f;

		_EnemyDirection[i] = EnemyDirection.GetSafeNormal();

		FVector BasePull = _EnemyDirection[i] * minForce;
		EnemyDirection = EnemyDirection.GetClampedToMaxSize(1000.0f);
		FVector Spring = EnemyDirection * stiffness;

		_EnemyVelocity[i] = (_EnemyVelocity[i] * DampingFactor) + (BasePull + Spring) * DeltaTime;
		_EnemyVelocity[i] = _EnemyVelocity[i].GetClampedToMaxSize2D(_speed[i]);

		float Phase = (Time * 18.0f) + (i * 1.7f);
		FVector BuzzVelocity(FMath::Cos(Phase), FMath::Sin(Phase * 1.3f), 0.0f);
		BuzzVelocity *= 600.0f;

		_EnemyPosition[i] += (_EnemyVelocity[i] + BuzzVelocity) * DeltaTime;
	}
}
