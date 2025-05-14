
// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterChar.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMeshSocket.h"
#include <DrawDebugHelpers.h>
#include "Particles/ParticleSystemComponent.h"
#include "Item.h"
#include "Components/WidgetComponent.h"
#include "Weapon.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
AShooterChar::AShooterChar() :
	BaseTurnRate(45.f),
	BaseLookUpRate(45.f),
	bAiming(false),
	CameraDefaultFOV(0.f),
	CameraZoomedFOV(35.f),
	ZoomInterpolationSpeed(20.f),
	CameraCurrentFOV(0.f),
	// turn rates for aiming/not aiming
	HipTurnRate(90.f),
	HipLookUpRate(90.f),
	AimingTurnRate(20.f),
	AimingLookUpRate(20.f),
	//Mouse look sensitivity
	MouseHipTurnRate(1.0f),
	MouseHipLookUpRate(1.0f),
	MouseAimingTurnRate(0.3f),
	MouseAimingLookUpRate(0.3f),
	CrosshairSpreadMultiplier(0.f),
	CrosshairVelocityFactor(0.f),
	CrosshairInAirFactor(0.f),
	CrosshairAimFactor(0.f),
	CrosshairShootingFactor(0.f),
	ShootTimeDuration(0.05f),
	bFiringBullet(false),
	AutomaticFireRate(0.1f),
	bShouldFire(true),
	bFireButtonPressed(false),
	bShouldTraceForItems(false)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/** Create CameraSpringArm */
	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	CameraSpringArm->SetupAttachment(RootComponent);
	CameraSpringArm->TargetArmLength = 200.f; // camera follows at this distance behind the char
	CameraSpringArm->bUsePawnControlRotation = true;
	CameraSpringArm->SocketOffset = FVector(0.f, 50.f, 65.f);

	/** Create FollowCamera*/
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraSpringArm, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;


	//DON"T ROTATE WHEN THE CONTROLLER ROTATE
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	//Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // character moves in the direction of input.
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f); // at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

}

// Called when the game starts or when spawned
void AShooterChar::BeginPlay()
{

	Super::BeginPlay();

	if (FollowCamera) {

		CameraDefaultFOV = GetFollowCamera()->FieldOfView;
		CameraCurrentFOV = CameraDefaultFOV;

	}
	//spawn default and equip it 
	EquipWeapon(SpawnDefaultWeapon());

}

void AShooterChar::MoveForward(float ThisValue)
{

	if ((Controller != nullptr) && (ThisValue != 0.0f)) {

		/** Find out which way is forward*/
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0 };

		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X) };
		AddMovementInput(Direction, ThisValue);

	}

}

void AShooterChar::MoveRight(float ThisValue)
{

	if ((Controller != nullptr) && (ThisValue != 0.0f)) {

		/** Find out which way is forward*/
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0 };

		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::Y) };
		AddMovementInput(Direction, ThisValue);

	}

}

void AShooterChar::TurnAtRate(float rate)
{

	AddControllerYawInput(rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());

}

void AShooterChar::LoopUpAtRate(float rate)
{

	AddControllerPitchInput(rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());

}

void AShooterChar::Turn(float Value)
{

	float TurnScaleFactor{};

	if (bAiming) {

		TurnScaleFactor = MouseAimingTurnRate;

	}
	else {

		TurnScaleFactor = MouseHipTurnRate;

	}

	AddControllerYawInput(Value * TurnScaleFactor);

}

void AShooterChar::LookUp(float Value)
{

	float LookUpScaleFactor{};

	if (bAiming) {

		LookUpScaleFactor = MouseAimingLookUpRate;

	}
	else {

		LookUpScaleFactor = MouseHipLookUpRate;

	}

	AddControllerPitchInput(Value * LookUpScaleFactor);

}

void AShooterChar::FireWeapon()
{

	if (FireSound) {
		UGameplayStatics::PlaySound2D(this, FireSound);
	}

	const USkeletalMeshSocket* BarrelSocket = GetMesh()->GetSocketByName("BarrelSocket");
	if (BarrelSocket) {
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(GetMesh());

		if (ParticleEffect) {
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleEffect, SocketTransform);
		}

		FVector BeamEnd;
		bool bBeamEnd = GetBeamEndLocation(SocketTransform.GetLocation(), BeamEnd);

		if (bBeamEnd) {

			//spawn impact particle after update beam end point...
			if (ImpactParticles) {

				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),
					ImpactParticles,
					BeamEnd
				);

			}

			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				BeamParticles,
				SocketTransform);

			if (Beam) {

				Beam->SetVectorParameter(FName("Target"), BeamEnd);

			}
		}

	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HipFireMontage) {

		AnimInstance->Montage_Play(HipFireMontage);
		AnimInstance->Montage_JumpToSection(FName("MontageSectionStartFire"));

	}

	StartCrosshairBulletFire();

}

bool AShooterChar::GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation)
{
	// check for crosshair hit
	FHitResult CrosshairHitResult;
	bool bCrosshairHit = TraceUndercrosshairs(CrosshairHitResult, OutBeamLocation);

	if (bCrosshairHit) {

		OutBeamLocation = CrosshairHitResult.Location;

	}
	else {



	}

	FHitResult WeaponTraceHit;
	const FVector WeaponTraceStart{ MuzzleSocketLocation };
	const FVector StartToEnd{ OutBeamLocation - MuzzleSocketLocation };
	const FVector WeaponTraceEnd{ MuzzleSocketLocation + StartToEnd * 1.25f };

	GetWorld()->LineTraceSingleByChannel(
		WeaponTraceHit,
		WeaponTraceStart,
		WeaponTraceEnd,
		ECollisionChannel::ECC_Visibility);

	if (WeaponTraceHit.bBlockingHit) {

		OutBeamLocation = WeaponTraceHit.Location;
		return true;
	}
	return false;

}

void AShooterChar::AimingButtonPressed()
{

	bAiming = true;

}

void AShooterChar::AimingButtonReleased()
{

	bAiming = false;

}

void AShooterChar::CameraInterpolationZoom(float DeltaTime)
{
	/**Set current camera fielf of view */
	if (bAiming) {
		//interpolate to zoom field of view
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV,
			CameraZoomedFOV,
			DeltaTime,
			ZoomInterpolationSpeed);

	}
	else {
		//interpolate to default field of view
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV,
			CameraDefaultFOV,
			DeltaTime,
			ZoomInterpolationSpeed);

	}

	GetFollowCamera()->SetFieldOfView(CameraCurrentFOV);
}

void AShooterChar::SetLookRates()
{

	if (bAiming) {
		BaseTurnRate = AimingTurnRate;
		BaseLookUpRate = AimingLookUpRate;
	}
	else {
		BaseTurnRate = HipTurnRate;
		BaseLookUpRate = HipLookUpRate;
	}

}

void AShooterChar::CalculateCrosshairSpread(float DeltaTime)
{

	FVector2D WalkSpeedRange{ 0.f, 600.f };
	FVector2D VelocityMultiplierRange{ 0.f , 1.f };
	FVector Velocity{ GetVelocity() };
	Velocity.Z = 0.f;

	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange,
		VelocityMultiplierRange,
		Velocity.Size());

	if (GetCharacterMovement()->IsFalling()) { // is in air? 

		CrosshairInAirFactor = FMath::FInterpTo(
			CrosshairInAirFactor,
			2.25f,
			DeltaTime,
			2.25f);

	}
	else {

		CrosshairInAirFactor = FMath::FInterpTo(
			CrosshairInAirFactor,
			0.f,
			DeltaTime,
			30.f);

	}

	if (bAiming) {

		CrosshairAimFactor = FMath::FInterpTo(
			CrosshairAimFactor,
			0.6f,
			DeltaTime,
			30.f);

	}
	else {

		CrosshairAimFactor = FMath::FInterpTo(
			CrosshairAimFactor,
			0.f,
			DeltaTime,
			30.f);

	}

	if (bFiringBullet) {

		CrosshairShootingFactor = FMath::FInterpTo(
			CrosshairShootingFactor,
			0.3f,
			DeltaTime,
			65.f
		);

	}
	else {

		CrosshairShootingFactor = FMath::FInterpTo(
			CrosshairShootingFactor,
			0.f,
			DeltaTime,
			65.f
		);

	}

	CrosshairSpreadMultiplier = 0.5f +
		CrosshairVelocityFactor +
		CrosshairInAirFactor - CrosshairAimFactor + CrosshairShootingFactor;

}

void AShooterChar::StartCrosshairBulletFire()
{

	bFiringBullet = true;

	GetWorldTimerManager().SetTimer(CrosshairShootTimer,
		this,
		&AShooterChar::FinishCrosshairBulletFire,
		ShootTimeDuration);

}

void AShooterChar::FinishCrosshairBulletFire()
{

	bFiringBullet = false;

}

void AShooterChar::FireButtonPressed()
{

	bFireButtonPressed = true;
	StartFireTimer();

}

void AShooterChar::FireButtonReleased()
{

	bFireButtonPressed = false;

}

void AShooterChar::StartFireTimer()
{

	if (bShouldFire) {
		FireWeapon();
		bShouldFire = false;
		GetWorldTimerManager().SetTimer(
			AutomaticFireTimer,
			this,
			&AShooterChar::AutomaticFireReset,
			AutomaticFireRate
		);
	}

}

void AShooterChar::AutomaticFireReset()
{

	bShouldFire = true;
	if (bFireButtonPressed) {
		StartFireTimer();
	}

}

bool AShooterChar::TraceUndercrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation)
{

	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport) {

		GEngine->GameViewport->GetViewportSize(ViewportSize);

	}

	//Get screen space location of crosshairs
	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	//get world position and direction of crosshair
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation, CrosshairWorldPosition, CrosshairWorldDirection);

	if (bScreenToWorld) {
		const FVector Start{ CrosshairWorldPosition };
		const FVector End{ Start + CrosshairWorldDirection * 50'000.f };
		OutHitLocation = End;
		GetWorld()->LineTraceSingleByChannel(OutHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility);

		if (OutHitResult.bBlockingHit) {

			OutHitLocation = OutHitResult.Location;
			return true;
		}
	}

	return false;
}

void AShooterChar::TraceForItems()
{

	if (bShouldTraceForItems) {
		FHitResult ItemTraceResult;
		FVector HitLocation;
		TraceUndercrosshairs(ItemTraceResult, HitLocation);

		if (ItemTraceResult.bBlockingHit) {

			TraceHitItem = Cast<AItem>(ItemTraceResult.Actor);
			if (TraceHitItem && TraceHitItem->GetPickupWidget()) {

				TraceHitItem->GetPickupWidget()->SetVisibility(true);

			}

			//we hit an AItem last frame
			if (TraceHitItemLastFrame) {
				if (TraceHitItem != TraceHitItemLastFrame) {
					TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
				}
			}

			TraceHitItemLastFrame = TraceHitItem;

		}
	}
	else if (TraceHitItemLastFrame) {

		//no longer overlapping any items
		TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
	}

}

AWeapon* AShooterChar::SpawnDefaultWeapon()
{
	//check the tsubclass of char
	if (DefaultWeaponClass) {
		//spawnweapon
		return GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);
	}

	return nullptr;

}

void AShooterChar::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip) {

		//hand socket
		const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(FName("RightHandSocket"));
		//attach to the hand 
		if (HandSocket) {
			HandSocket->AttachActor(WeaponToEquip, GetMesh());
		}
		EquipedWeapon = WeaponToEquip;
		EquipedWeapon->SetItemState(EItemState::EIS_Equipped);
	}

}

void AShooterChar::DropWeapon()
{

	if (EquipedWeapon) {

		FDetachmentTransformRules DetachementTransformRules(EDetachmentRule::KeepWorld, true);
		EquipedWeapon->GetItemMesh()->DetachFromComponent(DetachementTransformRules);
		EquipedWeapon->SetItemState(EItemState::EIS_Falling);
		EquipedWeapon->ThrowWeapon();
	}

}

void AShooterChar::SelectButtonPressed()
{
	if (TraceHitItem) {

		auto TraceHitWeapon = Cast<AWeapon>(TraceHitItem);
		SwapWeapon(TraceHitWeapon);
	}

}

void AShooterChar::SelectButtonReleased()
{



}

void AShooterChar::SwapWeapon(AWeapon* WeaponToSwap)
{

	DropWeapon();
	EquipWeapon(WeaponToSwap);
	TraceHitItem = nullptr;
	TraceHitItemLastFrame = nullptr;

}

// Called every frame
void AShooterChar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CameraInterpolationZoom(DeltaTime);
	SetLookRates();

	CalculateCrosshairSpread(DeltaTime);
	TraceForItems();
}

// Called to bind functionality to input
void AShooterChar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AShooterChar::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShooterChar::MoveRight);
	PlayerInputComponent->BindAxis("TurnRate", this, &AShooterChar::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AShooterChar::LoopUpAtRate);

	PlayerInputComponent->BindAxis("Turn", this, &AShooterChar::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AShooterChar::LookUp);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &AShooterChar::FireButtonPressed);
	PlayerInputComponent->BindAction("FireButton", IE_Released, this, &AShooterChar::FireButtonReleased);

	PlayerInputComponent->BindAction("AimingButton", IE_Pressed, this, &AShooterChar::AimingButtonPressed);
	PlayerInputComponent->BindAction("AimingButton", IE_Released, this, &AShooterChar::AimingButtonReleased);

	PlayerInputComponent->BindAction("Select", IE_Pressed, this, &AShooterChar::SelectButtonPressed);
	PlayerInputComponent->BindAction("Select", IE_Released, this, &AShooterChar::SelectButtonReleased);

}

float AShooterChar::GetCrosshairSpreadMultiplier() const
{
	return CrosshairSpreadMultiplier;
}

void AShooterChar::IncrementOverlappedItemCount(int8 Ammount)
{

	if (OverlappedItemCount + Ammount <= 0) {

		OverlappedItemCount = 0;
		bShouldTraceForItems = false;

	}
	else {

		OverlappedItemCount += Ammount;
		bShouldTraceForItems = true;

	}

}
