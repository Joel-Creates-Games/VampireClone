// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "WaveDirector.generated.h"

USTRUCT()
struct FWaveData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpawnTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 EnemyCount;

};

USTRUCT()
struct FAmbientWaveData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpawnAfter;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpawnBefore;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag SpawnTag;

};

UCLASS()
class AWaveDirector : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWaveDirector();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void WaveDataTimer(float DeltaTime);
	void AmbientDataTimer(float DeltaTime);
	void ManageActiveEnemies();

public:	
	UPROPERTY(EditAnywhere, Category = "Wave Setup")
	class UDataTable* WaveDataTable;
	UPROPERTY(EditAnywhere, Category = "Wave Setup")
	class UDataTable* AmbientDataTable;

	UPROPERTY(EditAnywhere, Category = "Enemy Settings")
	float SpawnTime = 2.0f;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Enemy Settings")
	class UCurveFloat* SpawnCurve = nullptr;

private:
	TArray<FWaveData> WaveDataArray;
	TArray<FAmbientWaveData> AmbientDataArray;
	TArray<FAmbientWaveData> ActiveEnemyTags;
	int32 CurrentWaveIndex = 0;
	float GameTime = 0.0f;

	class UCombatMessageSubsystem* CombatRadio = nullptr;

	float _spawnTimer = 0.0f;
	int nextEnemy = 0;
};
