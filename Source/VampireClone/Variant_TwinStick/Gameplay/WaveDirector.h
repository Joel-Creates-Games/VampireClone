// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
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

public:	
	UPROPERTY(EditAnywhere, Category = "Wave Setup")
	class UDataTable* WaveDataTable;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	TArray<FWaveData> WaveDataArray;
	int32 CurrentWaveIndex = 0;
	float GameTime = 0.0f;

	class UCombatMessageSubsystem* CombatRadio = nullptr;

};
