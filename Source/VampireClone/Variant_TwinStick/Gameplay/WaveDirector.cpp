// Fill out your copyright notice in the Description page of Project Settings.


#include "Variant_TwinStick/Gameplay/WaveDirector.h"
#include "CombatMessageSubsystem.h"

// Sets default values
AWaveDirector::AWaveDirector()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AWaveDirector::BeginPlay()
{
	Super::BeginPlay();

	CombatRadio = GetWorld()->GetSubsystem<UCombatMessageSubsystem>();

	TArray<FWaveData*> WaveDataPointerArray;
	WaveDataTable->GetAllRows<FWaveData>(FString("Wave Setup"), WaveDataPointerArray);
	for (FWaveData* WaveData : WaveDataPointerArray)
	{
		if(WaveData)
		{
			WaveDataArray.Add(*WaveData);
		}
	}
	WaveDataArray.Sort([](const FWaveData& A, const FWaveData& B) {
		return A.SpawnTime < B.SpawnTime;
	});

	TArray<FAmbientWaveData*> AmbientWaveDataPinterArray;
	AmbientDataTable->GetAllRows<FAmbientWaveData>(FString("Ambient Wave Setup"), AmbientWaveDataPinterArray);
	for (FAmbientWaveData* WaveData : AmbientWaveDataPinterArray)
	{
		if (WaveData)
		{
			AmbientDataArray.Add(*WaveData);
		}
	}
	AmbientDataArray.Sort([](const FAmbientWaveData& A, const FAmbientWaveData& B) {
		return A.SpawnAfter < B.SpawnAfter;
	});

	ActiveEnemyTags.Reserve(AmbientDataArray.Num());
}

void AWaveDirector::WaveDataTimer(float DeltaTime)
{
	GameTime += DeltaTime;
	if (CurrentWaveIndex <= WaveDataArray.Num()) { return; }
	if (GameTime > WaveDataArray[CurrentWaveIndex].SpawnTime)
	{
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Spawning Wave %d with %d enemies"), CurrentWaveIndex + 1, WaveDataArray[CurrentWaveIndex].EnemyCount));
		}
		CombatRadio->SwarmSpawnEnemies(WaveDataArray[CurrentWaveIndex].EnemyCount);
		CurrentWaveIndex++;
	}
}

void AWaveDirector::AmbientDataTimer(float DeltaTime)
{
	_spawnTimer += DeltaTime;
	if (!SpawnCurve) { return; }
	if (ActiveEnemyTags.Num() == 0) { return; }
	float currentTime = GetWorld()->GetTimeSeconds();
	if (_spawnTimer < SpawnTime / SpawnCurve->GetFloatValue(currentTime)) { return; }
	int choice = FMath::RandRange(0, ActiveEnemyTags.Num() - 1);
	CombatRadio->OnEnemySpawnType.Broadcast(ActiveEnemyTags[choice].SpawnTag);
	//SpawnEnemy(SetSpawnLocation());
	_spawnTimer = 0.0f;
}

void AWaveDirector::ManageActiveEnemies()
{
	float currentTime = GetWorld()->GetTimeSeconds();
	for (int i = ActiveEnemyTags.Num()-1; i >= 0; i--)
	{
		if (currentTime < ActiveEnemyTags[i].SpawnBefore) { continue; }
		ActiveEnemyTags[i] = ActiveEnemyTags[ActiveEnemyTags.Num() - 1];
		ActiveEnemyTags.Pop();
	}
	if (nextEnemy >= AmbientDataArray.Num()) { return; }
	if (AmbientDataArray[nextEnemy].SpawnAfter > currentTime) {
		ActiveEnemyTags.Add(AmbientDataArray[nextEnemy]);
		nextEnemy++;
	}
}


// Called every frame
void AWaveDirector::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	WaveDataTimer(DeltaTime);
	AmbientDataTimer(DeltaTime);
	ManageActiveEnemies();
}

