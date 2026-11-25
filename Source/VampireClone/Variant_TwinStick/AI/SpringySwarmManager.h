// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Variant_TwinStick/AI/SwarmManager.h"
#include "SpringySwarmManager.generated.h"

/**
 * 
 */
UCLASS()
class ASpringySwarmManager : public ASwarmManager
{
	GENERATED_BODY()


	virtual void ControlEnemy(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Enemy Settings")
	float stiffness = 0.1f;

	UPROPERTY(EditAnywhere, Category = "Enemy Settings")
	float Damping = 200.0f;

	UPROPERTY(EditAnywhere, Category = "Enemy Settings")
	float minForce = 600.0f;
	
};
