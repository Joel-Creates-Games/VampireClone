// Fill out your copyright notice in the Description page of Project Settings.


#include "Variant_TwinStick/AI/BibleAttack.h"

ABibleAttack::ABibleAttack()
{
}

void ABibleAttack::Setup()
{
	_projectileCount = 20;
	_baseSpeedModifier = 1.5f;
	Super::Setup();
}

void ABibleAttack::Behaviour(float DeltaTime)
{
	if (_isSpinning) {
		_currentSpinTime += DeltaTime;
		if (_currentSpinTime > _spinTime) {
			_isSpinning = false;
		}
		return;
	}
	Super::Behaviour(DeltaTime);
	if (_CurrentSpeed > _AttackSpeed) {
		_CurrentSpeed = 0;
		_isSpinning = true;
	}
}

void ABibleAttack::CalculateOrbitPositions()
{
	//we're ging to write this lin pure SIMD and it's going to be really hard
}
