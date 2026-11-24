// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Variant_TwinStick/AI/SwarmManager.h"
#include "LinearSwarmManager.generated.h"

/**
 * 
 */
UCLASS()
class ALinearSwarmManager : public ASwarmManager
{
	GENERATED_BODY()

	virtual void ControlEnemy(float DeltaTime) override;
	
};
