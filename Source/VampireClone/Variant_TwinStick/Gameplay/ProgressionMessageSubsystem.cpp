// Fill out your copyright notice in the Description page of Project Settings.


#include "Variant_TwinStick/Gameplay/ProgressionMessageSubsystem.h"
#include "UpgradeDefinition.h"
#include "ExpDrops.h"

void UProgressionMessageSubsystem::ExpDropsSpawnExp(FVector* position)
{
	if (!_exp) { return; }
	_exp->SpawnExp(*position);
}

void UProgressionMessageSubsystem::SetExpDrops(AExpDrops* exp)
{
	_exp = exp;
}
