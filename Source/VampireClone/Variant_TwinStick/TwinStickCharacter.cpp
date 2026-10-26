// Copyright Epic Games, Inc. All Rights Reserved.


#include "TwinStickCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "TwinStickGameMode.h"
#include "TwinStickAoEAttack.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "TwinStickProjectile.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "CombatMessageSubsystem.h"
#include "ProgressionMessageSubsystem.h"
#include "UpgradeDefinition.h"

ATwinStickCharacter::ATwinStickCharacter()
{
 	PrimaryActorTick.bCanEverTick = true;

	// create the spring arm
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(RootComponent);

	SpringArm->SetRelativeRotation(FRotator(-50.0f, 0.0f, 0.0f));

	SpringArm->TargetArmLength = 2200.0f;
	SpringArm->bDoCollisionTest = false;
	SpringArm->bInheritYaw = false;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 0.5f;

	// create the camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	Camera->SetFieldOfView(75.0f);

	// configure the character movement
	GetCharacterMovement()->GravityScale = 1.5f;
	GetCharacterMovement()->MaxAcceleration = 1000.0f;
	GetCharacterMovement()->BrakingFrictionFactor = 1.0f;
	GetCharacterMovement()->bCanWalkOffLedges = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 640.0f, 0.0f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
}

void ATwinStickCharacter::BeginPlay()
{
	Super::BeginPlay();

	// update the items count
	UpdateItems();
	//AActor* FoundActor = UGameplayStatics::GetActorOfClass(GetWorld(), ASwarmManager::StaticClass());
	//CachedSwarmManager = Cast<ASwarmManager>(FoundActor);
	//AActor* FoundMagicWandController = UGameplayStatics::GetActorOfClass(GetWorld(), AMagicWandController::StaticClass());
	//CachedWandController = Cast<AMagicWandController>(FoundMagicWandController);
	//AActor* FoundExpDrops = UGameplayStatics::GetActorOfClass(GetWorld(), AExpDrops::StaticClass());
	//CachedExpDrop = Cast<AExpDrops>(FoundExpDrops);
	UCombatMessageSubsystem* CombatRadio = GetWorld()->GetSubsystem<UCombatMessageSubsystem>();
	if (CombatRadio) {
		CombatRadio->OnPlayerHit.AddDynamic(this, &ATwinStickCharacter::TwinStickTakeDamage);
		CombatRadio->SetTwinStickCharacter(this);
	}
	ProgressRadio = GetWorld()->GetSubsystem<UProgressionMessageSubsystem>();
	if (ProgressRadio) {
		ProgressRadio->OnExperienceGain.AddDynamic(this, &ATwinStickCharacter::TwinStickGainExperience);
		//ProgressRadio->OnC
		//ProgressionRadio->OnApplyPowerup.AddDynamic(this, &ATwinStickCharacter::TwinStickApplyPowerup);
	}
	/*if (!CachedSwarmManager) {
		UE_LOG(LogTemp, Warning, TEXT("SwarmManager not found in the world!"));
	}*/
	/*if (!CachedWandController) {
		UE_LOG(LogTemp, Warning, TEXT("MagicWandController not found in the world!"));
	}*/
	/*if (!CachedExpDrop) {
		UE_LOG(LogTemp, Warning, TEXT("ExpDrops not found in the world!"));
	}*/

	Health = MaxHealth;
}

void ATwinStickCharacter::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	/** Clear the autofire timer */
	GetWorld()->GetTimerManager().ClearTimer(AutoFireTimer);
}

void ATwinStickCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// set the player controller reference
	PlayerController = Cast<APlayerController>(GetController());
}

void ATwinStickCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// get the current rotation
	const FRotator OldRotation = GetActorRotation();

	// are we aiming with the mouse?
	if (bUsingMouse)
	{
		if (PlayerController)
		{
			// get the cursor world location
			FHitResult OutHit; 
			PlayerController->GetHitResultUnderCursorByChannel(MouseAimTraceChannel, true, OutHit);

			// find the aim rotation 
			const FRotator AimRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), OutHit.Location);

			// save the aim angle
			AimAngle = AimRot.Yaw;

			

			// update the yaw, reuse the pitch and roll
			SetActorRotation(FRotator(OldRotation.Pitch, AimAngle, OldRotation.Roll));

		}

	} else {

		// use quaternion interpolation to blend between our current rotation
		// and the desired aim rotation using the shortest path
		const FRotator TargetRot = FRotator(OldRotation.Pitch, AimAngle, OldRotation.Roll);

		SetActorRotation(TargetRot);
	}

	//MagicWand(DeltaTime);
	//CheckCollisions();
}

void ATwinStickCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// set up the enhanced input action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATwinStickCharacter::Move);
		EnhancedInputComponent->BindAction(StickAimAction, ETriggerEvent::Triggered, this, &ATwinStickCharacter::StickAim);
		EnhancedInputComponent->BindAction(MouseAimAction, ETriggerEvent::Triggered, this, &ATwinStickCharacter::MouseAim);
		//EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, this, &ATwinStickCharacter::Dash);
		//EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Triggered, this, &ATwinStickCharacter::Shoot);
		//EnhancedInputComponent->BindAction(AoEAction, ETriggerEvent::Triggered, this, &ATwinStickCharacter::AoEAttack);

	}

}

void ATwinStickCharacter::Move(const FInputActionValue& Value)
{
	// save the input vector
	FVector2D InputVector = Value.Get<FVector2D>();

	// route the input
	DoMove(InputVector.X, InputVector.Y);
}

void ATwinStickCharacter::StickAim(const FInputActionValue& Value)
{
	// get the input vector
	FVector2D InputVector = Value.Get<FVector2D>();

	// route the input
	DoAim(InputVector.X, InputVector.Y);
}

void ATwinStickCharacter::MouseAim(const FInputActionValue& Value)
{
	// raise the mouse controls flag
	bUsingMouse = true;

	// show the mouse cursor
	if (PlayerController)
	{
		PlayerController->SetShowMouseCursor(true);
	}
}

void ATwinStickCharacter::Dash(const FInputActionValue& Value)
{
	// route the input
	DoDash();
}

void ATwinStickCharacter::Shoot(const FInputActionValue& Value)
{
	// route the input
	DoShoot();
}

void ATwinStickCharacter::AoEAttack(const FInputActionValue& Value)
{
	// route the input
	DoAoEAttack();
}

void ATwinStickCharacter::TwinStickTakeDamage(float Damage)
{
	Health = FMath::Max(0, Health - Damage);
	OnHealthChanged.Broadcast(Health, MaxHealth);

	if (Health <= 0.0f) {
		UE_LOG(LogTemp, Warning, TEXT("Player Died!"));
	}
}

void ATwinStickCharacter::TwinStickGainExperience(float exp)
{
	_totalExperience = FMath::Min(_levelUpExp, _totalExperience + exp);
	if (_totalExperience >= _levelUpExp) {
		_totalExperience = 0;
		_levelUpExp *= 1.5f;
		UE_LOG(LogTemp, Warning, TEXT("Player Levelled!"));
		_level++;
		OnLevelUp.Broadcast(_level);
	}
	OnExpChanged.Broadcast(_totalExperience, _levelUpExp);
}



void ATwinStickCharacter::DoMove(float AxisX, float AxisY)
{
	// save the input
	LastMoveInput.X = AxisX;
	LastMoveInput.Y = AxisY;

	// calculate the forward component of the input
	FRotator FlatRot = GetControlRotation();
	FlatRot.Pitch = 0.0f;

	// apply the forward input
	AddMovementInput(FlatRot.RotateVector(FVector::ForwardVector), AxisX);

	// apply the right input
	AddMovementInput(FlatRot.RotateVector(FVector::RightVector), AxisY);
}

void ATwinStickCharacter::DoAim(float AxisX, float AxisY)
{
	// calculate the aim angle from the inputs
	AimAngle = FMath::RadiansToDegrees(FMath::Atan2(AxisY, -AxisX));

	// lower the mouse controls flag
	bUsingMouse = false;

	// hide the mouse cursor
	if (PlayerController)
	{
		PlayerController->SetShowMouseCursor(false);
	}

	// are we on autofire cooldown?
	if (!bAutoFireActive)
	{
		// set ourselves on cooldown
		bAutoFireActive = true;

		// fire a projectile
		DoShoot();

		// schedule autofire cooldown reset
		GetWorld()->GetTimerManager().SetTimer(AutoFireTimer, this, &ATwinStickCharacter::ResetAutoFire, AutoFireDelay, false);
	}
}

void ATwinStickCharacter::DoDash()
{
	// calculate the launch impulse vector based on the last move input
	FVector LaunchDir = FVector::ZeroVector;

	LaunchDir.X = FMath::Clamp(LastMoveInput.X, -1.0f, 1.0f);
	LaunchDir.Y = FMath::Clamp(LastMoveInput.Y, -1.0f, 1.0f);

	// launch the character in the chosen direction
	LaunchCharacter(LaunchDir * DashImpulse, true, true);
}

void ATwinStickCharacter::DoShoot()
{
	// get the actor transform
	FTransform ProjectileTransform = GetActorTransform();

	// apply the projectile spawn offset
	FVector ProjectileLocation = ProjectileTransform.GetLocation() + ProjectileTransform.GetRotation().RotateVector(FVector::ForwardVector * ProjectileOffset);
	ProjectileTransform.SetLocation(ProjectileLocation);

	// ensure we don't spawn a projectile if it collides with a wall or other nearby obstacle
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;

	ATwinStickProjectile* Projectile = GetWorld()->SpawnActor<ATwinStickProjectile>(ProjectileClass, ProjectileTransform, SpawnParams);
}

void ATwinStickCharacter::DoAoEAttack()
{
	// do we have enough items to do an AoE attack?
	if (Items > 0)
	{
		// get the game time
		const float GameTime = GetWorld()->GetTimeSeconds();

		// are we off AoE cooldown?
		if (GameTime - LastAoETime > AoECooldownTime)
		{
			// save the new AoE time
			LastAoETime = GameTime;

			// spawn the AoE
			ATwinStickAoEAttack* AoE = GetWorld()->SpawnActor<ATwinStickAoEAttack>(AoEAttackClass, GetActorTransform());

			// decrease the number of items
			--Items;

			// update the items count
			UpdateItems();
		}
	}
}

void ATwinStickCharacter::HandleDamage(float Damage, const FVector& DamageDirection)
{
	// calculate the knockback vector
	FVector LaunchVector = DamageDirection;
	LaunchVector.Z = 0.0f;

	// apply knockback to the character
	LaunchCharacter(LaunchVector * KnockbackStrength, true, true);

	// pass control to BP
	BP_Damaged();
}

FVector* ATwinStickCharacter::GetPosition()
{
	_position = GetActorLocation();
	return &_position;
}

void ATwinStickCharacter::AddPickup()
{
	// increase the item count
	++Items;

	// update the items counter
	UpdateItems();
}

void ATwinStickCharacter::AddExperience(float Experience)
{
	_totalExperience += Experience;
}

void ATwinStickCharacter::CheckCollisions()
{
	if (!ProgressRadio) { return; }

	//_totalExperience += ProgressRadio->GetExpDrops().;
}

void ATwinStickCharacter::UpdateItems()
{
	// update the game mode
	if (ATwinStickGameMode* GM = Cast<ATwinStickGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GM->ItemUsed(Items);
	}
}

void ATwinStickCharacter::ResetAutoFire()
{
	// reset the autofire flag
	bAutoFireActive = false;
}

