// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "EnemyDataDefinition.generated.h"

/**
 * 
 */
UCLASS()
class UEnemyDataDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Stats")
	FString name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Stats")
	float health;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Stats")
	float speed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visuals")
	class UStaticMesh* mesh;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Broadcast")
	FGameplayTag enemyTag;
};
