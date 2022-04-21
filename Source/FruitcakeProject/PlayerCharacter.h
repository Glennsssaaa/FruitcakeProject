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
	
	void DashCooldown();
	
	/* --------- Switch Perspective ---------- */
	UFUNCTION(BlueprintCallable, Category = "Perspective")
		void SwitchPerspectiveMethod(float value);


	/* --------- Player Projectile Casting ---------- */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
		void CastProjectileMethod();

	void ResetProjectile();

	void RotatePlayerToCursor();
	
	bool GetCanDamage();
	void SetCanDamage();

protected:

	// Projectile muzzle offset from the camera location
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector MuzzleOffset;

	float m_Turn_Rate;
	float m_Look_Rate;
	bool is_Dashing;


	/* ------ Player Melee Attack ------ */
	


	/* ------- Player Spell Cast Attack ------ */
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Variables")
	bool is_Spell_cast;

	// Timer handle to handle projectile cooldown
	FTimerHandle SpellcastTimerHandle;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell Cast")
	bool can_Cast;

	/* ------ Player Dash ------ */

	// Can the player currently dash (are they already currently dashing)
	bool bCan_Dash;
	
	// Dash Timer
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTimerHandle t_Dash_Function;
	
	// How far the player will dash
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float m_Dash_Distance;

	// How fast the player will dash
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float m_Dash_Speed;

	// How often the player can dash
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float m_Dash_Cooldown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTimerHandle t_Dash_Cooldown_Timer;
	
	// Stores the players initial position
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector m_Base_Location;

	// Stores the players predicted position for after the dash is complete
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector m_Predicted_Location;
	



	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool m_Can_Move = true;
	
	/* ------ Player Camera ------ */

	// Spring Arm Component for controlling the camera
	UPROPERTY(EditAnywhere, BlueprintReadWrite) 		
	USpringArmComponent* CameraBoom;
	
	// Camera Component
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly) 	
	UCameraComponent* Camera;

	// Camera Rotator
	UPROPERTY(EditAnywhere, BlueprintReadWrite) 		
	FRotator m_Cam_Rotate;

	// Current Camera Angle
	UPROPERTY(EditAnywhere, BlueprintReadWrite)			
	float m_Rotation_Angle = 0.f;

	// Target Angle
	UPROPERTY(EditAnywhere, BlueprintReadWrite)			
	float m_Target_Angle = 45.f;
	
	// Camera Rotation Speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite)			
	float m_Rotation_Speed = 200.f;

	float m_Camera_Zoom_Value;

	
	/* ------- Box Push ------- */
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float moveDir = 0.f;

	// Animation Blueprint Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Variables")
	bool is_Attack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Variables")
	bool can_Damage;
	

};