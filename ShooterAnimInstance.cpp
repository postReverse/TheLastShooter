// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"
#include "ShooterChar.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UShooterAnimInstance::UpdateAnimationProperties(float DeltaTime)
{

	if (ShooterChar == nullptr) {

		ShooterChar = Cast<AShooterChar>(TryGetPawnOwner());
	
	}

	if (ShooterChar) {
		
		//get speed of char 
		FVector Velocity{ ShooterChar->GetVelocity() };
		Velocity.Z = 0;
		Speed = Velocity.Size();

		// is the character in air?
		bIsInAir = ShooterChar->GetCharacterMovement()->IsFalling();

		//is the character accelerating?
		if (ShooterChar->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0) {
			bIsAccelerating = true;
		}
		else {
			bIsAccelerating = false; 
		}

		FRotator AimRotation = ShooterChar->GetBaseAimRotation();
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(ShooterChar->GetVelocity());
		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;


		if (ShooterChar->GetVelocity().Size() > 0.f) {
			LastMovementOffsetYaw = MovementOffsetYaw;
		}

		bAiming = ShooterChar->GetAiming();

	}

}

void UShooterAnimInstance::NativeInitializeAnimation()
{

	ShooterChar = Cast<AShooterChar>(TryGetPawnOwner());

}
