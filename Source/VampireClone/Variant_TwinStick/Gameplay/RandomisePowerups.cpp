// Fill out your copyright notice in the Description page of Project Settings.


#include "Variant_TwinStick/Gameplay/RandomisePowerups.h"
#include "UpgradeDefinition.h"

TArray<UUpgradeDefinition*> URandomisePowerups::GetRandomUpgrades(int Count)
{
	TArray<UUpgradeDefinition*> chosen;
	chosen.Reserve(Count);
	int choice;
	for (int i = 0; i < Count; i++)
	{
		choice = FMath::RandRange(0, AllUpgrades.Num() - 1);
		chosen.Add(AllUpgrades[choice]);
	}
	return chosen;
}
