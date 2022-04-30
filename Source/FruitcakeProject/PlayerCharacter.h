// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ParticleDefinitions.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerCharacter.generated.h"

UCLASS(Blueprintable)
class FRUITCAKEPROJECT_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// AOE attack spawn class (Vector)
	UPROPERTY(EditDefaultsOnly, Category = "PlayerProjectiles")
	TSubclassOf<class AProjectiles> SpellClass;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/* --------- Movement Inputs ---------- */

	// Forward and Backward Movement
	UFUNCTION() 
	void MoveForwardMethod(float Value);

	// Right and Left Movement
	UFUNCTION() 
	void MoveRightMethod(float Value);
	
	UFUNCTION()
	void TurnAtRateMethod(float Value);
	
	UFUNCTION()
	void LookUpRateMethod(float Value);

	/* --------- Dash Functions ---------- */

	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool ImprovedDashFunction();
	
	void DashCooldownFunction();
	
	/* --------- Switch Perspective ---------- */
	UFUNCTION(BlueprintCallable, Category = "Perspective")
	void SwitchPerspectiveMethod(float value);


	/* ------ Player Melee Combat ------ */
	UFUNCTION()
	void MeleeAttack();

	void MeleeAttackCooldown();
	
	/* --------- Player Projectile Casting ---------- */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void CastProjectileMethod();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void CastProjectile();
	
	void ResetProjectile();

	void RotatePlayerToCursor();
	
	bool GetCanDamage();
	void SetCanDamage();

protected:

	// Projectile muzzle offset from the camera location
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector MuzzleOffset;

	float TurnRate;
	float LookRate;
	bool bIsDashing;
	bool perftest;
	// Camera Tracker Object
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* CameraTracker;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    USceneComponent* Root;
	/* ------ Player Melee Attack ------ */
	float MeleeCooldownTimer;
	FTimerHandle MeleeTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Variables")
	bool bIsAttack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Variables")
	bool bCanDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Variables")
	bool bCanAttack;

	
	/* ------- Player Spell Cast Attack ------ */
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Variables")
	bool bIsSpellCast;

	// Timer handle to handle projectile cooldown
	FTimerHandle CastTimerHandle;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell Cast")
	bool bCanCast;

	/* ------ Player Dash ------ */

	// Can the player currently dash (are they already currently dashing)
	bool bCanDash;
	
	// Dash Timer
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTimerHandle DashTimer;
	
	// How far the player will dash
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DashDistance;

	// How fast the player will dash
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DashSpeed;

	// How often the player can dash
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DashCooldown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTimerHandle DashCooldownTimer;
	
	// Stores the players initial position
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector BaseLocation;

	// Stores the players predicted position for after the dash is complete
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector PredictedLocation;
	



	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bCanMove = true;
	
	/* ------ Player Camera ------ */

	// Spring Arm Component for controlling the camera
	UPROPERTY(EditAnywhere, BlueprintReadWrite) 		
	USpringArmComponent* CameraBoom;
	
	// Camera Component
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly) 	
	UCameraComponent* Camera;

	// Camera Rotator
	UPROPERTY(EditAnywhere, BlueprintReadWrite) 		
	FRotator CameraRotate;

	// Current Camera Angle
	UPROPERTY(EditAnywhere, BlueprintReadWrite)			
	float RotationAngle = 0.f;

	// Target Angle
	UPROPERTY(EditAnywhere, BlueprintReadWrite)			
	float TargetAngle = 45.f;
	
	// Camera Rotation Speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite)			
	float RotationSpeed = 200.f;

	float CameraZoomValue;
	/* ------- Box Push ------- */
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MoveDirection = 0.f;
	
};