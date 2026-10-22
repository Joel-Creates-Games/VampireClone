// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ProgressionMessageSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnExperienceGain, float, GainAmount);

UCLASS()
class UProgressionMessageSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	FOnExperienceGain OnExperienceGain;
	
};
