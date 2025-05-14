// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterChar.generated.h"

UCLASS()
class THELASTSHOOTER_API AShooterChar : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterChar();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float ThisValue); //Move forward and backwards.
	void MoveRight(float ThisValue); //Side to side input.

	void TurnAtRate(float rate); // Called the input to turn at a given rate.
	void LoopUpAtRate(float rate); // Called the input to look up/down at a given rate. 


	void Turn(float Value);

	void LookUp(float Value);


	// Called when the fire button is pressed.
	void FireWeapon();

	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation);

	/**Set bAiming to true of false */
	void AimingButtonPressed();
	void AimingButtonReleased();
	void CameraInterpolationZoom(float DeltaTime);
	void SetLookRates();


	void CalculateCrosshairSpread(float DeltaTime);

	void StartCrosshairBulletFire();

	UFUNCTION()
	void FinishCrosshairBulletFire();


	void FireButtonPressed();
	void FireButtonReleased();

	void StartFireTimer();
	UFUNCTION()
	void AutomaticFireReset();

	bool TraceUndercrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation);

	void TraceForItems();

	//Spawn weapon and equip it 
	class AWeapon* SpawnDefaultWeapon();
	//takes a weapon and attaches it to the mesh 
	void EquipWeapon( AWeapon* WeaponToEquip);

	void DropWeapon();

	void SelectButtonPressed();
	void SelectButtonReleased();

	//Drop and equip 
	void SwapWeapon(AWeapon* WeaponToSwap);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	/** CameraSpringArm position behind the character. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraSpringArm;

	/**Camera that follow the char*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** Base turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseTurnRate;

	/** Base Look Up rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseLookUpRate;

	/** Turn rate while not aiming */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float HipTurnRate; 
	
	/* Look Up Rate when not aiming */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float HipLookUpRate;
	
	/* Turn rate when aiming */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float AimingTurnRate; 
	
	/* Look up Rate when aiming*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float AimingLookUpRate; 

	/* Turn rate when not aiming */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseHipTurnRate;

	/*Look Up Rate when not aiming*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMAX = "1.0"))
	float MouseHipLookUpRate;

	/*Turn rate when Aiming*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMAX = "1.0"))
	float MouseAimingTurnRate;

	/*Look up rate when aiming*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMAX = "1.0"))
	float MouseAimingLookUpRate; 


	/** Montage fire weapon*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* HipFireMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class USoundCue* FireSound; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* ParticleEffect;

	//Particles spawn in fire weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticles; 

	//smoke trail for bullets.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BeamParticles;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bAiming;

	/** Default camera FOV */
	float CameraDefaultFOV;

	/** FOV when zoom */
	float CameraZoomedFOV;

	/**Current FOV this FRAME*/
	float CameraCurrentFOV; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combat, meta = (AllowPrivateAccess = "true"))
	float ZoomInterpolationSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairSpreadMultiplier;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairVelocityFactor; 
	
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairInAirFactor; 

	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairAimFactor; 

	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairShootingFactor; 


	float ShootTimeDuration;
	bool bFiringBullet;
	FTimerHandle CrosshairShootTimer;

	bool bFireButtonPressed; 
	bool bShouldFire; 
	float AutomaticFireRate; 
	FTimerHandle AutomaticFireTimer; 


	bool bShouldTraceForItems;

	int8 OverlappedItemCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = true ))
	class AItem* TraceHitItemLastFrame;

	//currently equiped weapon
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = true ))
	AWeapon* EquipedWeapon; 

	//default weapon 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = true ))
	TSubclassOf<AWeapon> DefaultWeaponClass; 


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = true ))
	AItem* TraceHitItem; 

public:
	/** Return a subobject*/
	FORCEINLINE USpringArmComponent* GetCameraSpringArm() const { return CameraSpringArm; }

	/** Return FollowCamera subobject*/
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE bool GetAiming() const { return bAiming; }


	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpreadMultiplier() const; 

	FORCEINLINE int8 GetOverlappedItemCount() { return OverlappedItemCount; }

	void IncrementOverlappedItemCount(int8 Ammount);

};