// Fill out your copyright notice in the Description page of Project Settings.


#include "Variant_TwinStick/Gameplay/CombatMessageSubsystem.h"
#include "SwarmManager.h"
#include "TwinStickCharacter.h"

FVector* UCombatMessageSubsystem::GetSwarmEnemyPosition(int pos)
{
	if (_swarm == nullptr) { return nullptr; }
	return _swarm->GetEnemyPosition(pos);
}

void UCombatMessageSubsystem::SetSwarmManager(ASwarmManager* swarm)
{
	_swarm = swarm;
}

void UCombatMessageSubsystem::SetTwinStickCharacter(ATwinStickCharacter* player)
{
	_player = player;
}

int UCombatMessageSubsystem::GetClosestSwarm(FVector location)
{
	if (_swarm == nullptr) { return -1; }
	return _swarm->GetClostestEnemyIndex(location);
}

void UCombatMessageSubsystem::SwarmTakeDamage(int index, float damage)
{
	if (_swarm == nullptr) { return; }
	_swarm->TakeDamage(index, damage);
}

FVector* UCombatMessageSubsystem::GetPlayerPosition()
{
	if (_player == nullptr) { return nullptr; }
	return _player->GetPosition();
}
