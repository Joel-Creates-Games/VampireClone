// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExpDrops.generated.h"

class UStaticMeshComponent;
class UInstancedStaticMeshComponent;
UCLASS()
class VAMPIRECLONE_API AExpDrops : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExpDrops();

	//UPROPERTY(VisibleAnywhere, Category = "References")
	//APawn* CachedPlayerPawn = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UInstancedStaticMeshComponent* ISMComponent;

private:
	class UCombatMessageSubsystem* _CombatRadio = nullptr;

	TArray<int> _deadExp;
	static const int _expCount = 100;
	FVector _ExpDirection[_expCount];
	FVector _ExpPosition[_expCount];
	FVector _ExpVelocity[_expCount];

	float _timeLeft[_expCount];
	float _currentSpeed[_expCount];
	bool _alive[_expCount];

	float _maxSpeed = 1000.0f;
	float _expValue = 1;
	float _expValueModifier = 1.0f;
	float _attractDistance = 400 * 400;
	float _hitDistance = 100 * 100;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void Setup();
	void MoveExp(float DeltaTime);
	void KillExp(const int Exp);
	void DrawExpMesh();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	int GetNextExp();
	float GetExpValue() const { return _expValue * _expValueModifier; }
	void SpawnExp(const FVector& SpawnLocation);

	float CheckPlayerCollision();


};
