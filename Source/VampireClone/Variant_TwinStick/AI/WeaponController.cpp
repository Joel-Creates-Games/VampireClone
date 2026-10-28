// Fill out your copyright notice in the Description page of Project Settings.


#include "Variant_TwinStick/AI/WeaponController.h"

// Sets default values
AWeaponController::AWeaponController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AWeaponController::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWeaponController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

