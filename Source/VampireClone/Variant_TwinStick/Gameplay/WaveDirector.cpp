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
}

// Called every frame
void AWaveDirector::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (CurrentWaveIndex <= WaveDataArray.Num()) { return; }
	GameTime += DeltaTime;
	if(GameTime > WaveDataArray[CurrentWaveIndex].SpawnTime)
	{
		if(GEngine){
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Spawning Wave %d with %d enemies"), CurrentWaveIndex + 1, WaveDataArray[CurrentWaveIndex].EnemyCount));
		}
		CombatRadio->SwarmSpawnEnemies(WaveDataArray[CurrentWaveIndex].EnemyCount);
		CurrentWaveIndex++;
	}
}

