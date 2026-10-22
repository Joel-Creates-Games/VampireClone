// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CombatMessageSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerHitSigniture, float, DamageAmount);

UCLASS()
class UCombatMessageSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FOnPlayerHitSigniture OnPlayerHit;
};
