// Fill out your copyright notice in the Description page of Project Settings.


#include "ExpDrops.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "ProgressionMessageSubsystem.h"
#include "CombatMessageSubsystem.h"

// Sets default values
AExpDrops::AExpDrops()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ISMComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("ISMComponent"));
	RootComponent = ISMComponent;
	ISMComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void AExpDrops::BeginPlay()
{
	Super::BeginPlay();
	Setup();
}

void AExpDrops::Setup()
{
	UWorld* world = GetWorld();
	if (!ensure(world)) return;
	APlayerController* playerController = world->GetFirstPlayerController();
	if (!ensure(playerController)) return;
	//CachedPlayerPawn = playerController->GetPawn();
	//if (!ensure(CachedPlayerPawn)) return;
	UProgressionMessageSubsystem* ProgressRadio = GetWorld()->GetSubsystem<UProgressionMessageSubsystem>();
	if (ProgressRadio) {
		ProgressRadio->SetExpDrops(this);
	}
	_CombatRadio = GetWorld()->GetSubsystem<UCombatMessageSubsystem>();

	//_deadExp.Reserve(_expCount);

	FTransform HiddenTransform;
	HiddenTransform.SetLocation(FVector(0, 0, -1000.0f));

	//TArray<FTransform> InstanceTransforms;
	_InstanceTransforms.Init(HiddenTransform, _expCount);

	_ExpDirection.Init(FVector(0, 0, 0), _expCount);
	_ExpPosition.Init(FVector(0, 0, 0), _expCount);
	_ExpVelocity.Init(FVector(0, 0, 0), _expCount);
	_timeLeft.Init(0.0f, _expCount);
	_currentSpeed.Init(0.0f, _expCount);

	ISMComponent->AddInstances(_InstanceTransforms, false);
}

void AExpDrops::MoveExp(float DeltaTime)
{
	if (!_CombatRadio) { return; }
	FVector* playerPos = _CombatRadio->GetPlayerPosition();
	for(int i = 0; i < _aliveEXPCount; i++)
	{
		//if (!_alive[i]) { continue;}
		float distanceToPlayer = FVector::DistSquared(_ExpPosition[i], *playerPos);
		_timeLeft[i] -= DeltaTime;
		if (_timeLeft[i] <= 0.0f)
		{
			KillExp(i);
			continue;
		}
		if (distanceToPlayer > _attractDistance) 
		{
			_currentSpeed[i] = FMath::Max(_currentSpeed[i] - 1000.0f * DeltaTime, 0.0f);
			_ExpVelocity[i] = _ExpDirection[i] * _currentSpeed[i];
			_ExpPosition[i] -= _ExpVelocity[i] * DeltaTime;
			continue; 
		}
		_ExpDirection[i] = (*playerPos - _ExpPosition[i]).GetSafeNormal();
		_currentSpeed[i] = FMath::Min(_currentSpeed[i] + 1000.0f * DeltaTime, _maxSpeed);
		_ExpVelocity[i] = _ExpDirection[i] * _currentSpeed[i];
		_ExpPosition[i] += _ExpVelocity[i] * DeltaTime;
	}
}

float AExpDrops::CheckPlayerCollision()
{
	FVector* playerPos = _CombatRadio->GetPlayerPosition();
	float totalGain = 0;
	UProgressionMessageSubsystem* ProgressionRadio = GetWorld()->GetSubsystem<UProgressionMessageSubsystem>();
	float fullValue = _expValue * _expValueModifier;
	for (int i = 0; i < _aliveEXPCount;)
	{
		//if (!_alive[i]) { continue; }
		if (FVector::DistSquared(_ExpPosition[i], *playerPos) < _hitDistance)
		{
			ProgressionRadio->OnExperienceGain.Broadcast(fullValue);
			KillExp(i);
			totalGain += GetExpValue();
		}
		else
		{
			i++;
		}
	}
	return totalGain;
}

void AExpDrops::KillExp(const int Exp)
{
	//if (!_alive[Exp]) { return; }
	//_alive[Exp] = false;
	//_deadExp.Push(Exp);
	SwapSOA(Exp, _aliveEXPCount-1);
	_InstanceTransforms[_aliveEXPCount-1].SetLocation(FVector(0, 0, -999999.0f));
	_aliveEXPCount--;
}

void AExpDrops::DrawExpMesh()
{
	for (int i = 0; i < _aliveEXPCount; i++)
	{
		//FTransform ExpTransform;
		//if (_alive[i]) {
			//ExpTransform.SetLocation(_ExpPosition[i]);
		//}
		//else
		//{
			//FVector HiddenPos = _ExpPosition[i];
			//HiddenPos.Z = -500.0f; // Hidden underground
			//ExpTransform.SetLocation(HiddenPos);
		//}
		_InstanceTransforms[i].SetLocation(_ExpPosition[i]);
	}
	ISMComponent->BatchUpdateInstancesTransforms(0, _InstanceTransforms, true, false, true);
}

void AExpDrops::SwapSOA(int indexA, int indexB)
{
	_ExpPosition[indexA] = _ExpPosition[indexB];
	_ExpDirection[indexA] = _ExpDirection[indexB];
	_ExpVelocity[indexA] = _ExpVelocity[indexB];
	_timeLeft[indexA] = _timeLeft[indexB];
	_currentSpeed[indexA] = _currentSpeed[indexB];
}

void AExpDrops::CleanupExp()
{
	CheckPlayerCollision();
}

// Called every frame
void AExpDrops::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MoveExp(DeltaTime);
	DrawExpMesh();
	CleanupExp();
}

int AExpDrops::GetNextExp()
{
	if (_aliveEXPCount == _expCount) { return -1; }
	//int nextExp = _deadExp.Pop();
	//_alive[nextExp] = true;
	_timeLeft[_aliveEXPCount] = 10.0f;
	_aliveEXPCount++;
	return _aliveEXPCount -1;
}

void AExpDrops::SpawnExp(const FVector& SpawnLocation)
{
	int index = GetNextExp();
	if (index == -1) { return; }
	//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Purple, FString::Printf(TEXT("exp tried spawn")));
	_ExpPosition[index] = SpawnLocation;
	_ExpVelocity[index] = FVector::ZeroVector;
	_currentSpeed[index] = 0.0f;
}

