// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ParticleDefinitions.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
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
	UPROPERTY(EditDefaultsOnly, Category = "AOE")
		TSubclassOf<class AAoeAttackController> AOEAttackClass;

	// AOE attack spawn class (Vector)
	UPROPERTY(EditDefaultsOnly, Category = "PlayerProjectiles")
		TSubclassOf<class AProjectiles> ProjectileClass;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/* --------- Movement Inputs ---------- */

	UFUNCTION() // Forward and Backward Movement
		void MoveForwardMethod(float value);
	UFUNCTION() // Right and Left Movement
		void MoveRightMethod(float value);
	UFUNCTION(BlueprintCallable)
		void TurnAtRateMethod(float value);
	UFUNCTION(BlueprintCallable)
		void LookUpRateMethod(float value);

	/* --------- Dash Functions ---------- */

	// Basic, needs to be changed 
	UFUNCTION()
		void DashInputMethod();
	void DashMethod();
	UFUNCTION()
		void ResetDashMethod();

	/* --------- Switch Persepctive ---------- */
	UFUNCTION(BlueprintCallable, Category = "Perspective")
		void SwitchPerspectiveMethod(float value);


	/* --------- Player Projectile Casting ---------- */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
		void CastProjectileMethod();

	void ResetProjecitle();

	/* --------- Test Functions ---------- */
	UFUNCTION()
		void FireAoeAtPlayer();

	UFUNCTION()
		void FireABiggerAoe();


	void RotatePlayerToCursor();

	void ReducePlayerHealth();


	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);
protected:

	// Gun muzzle offset from the camera location
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		FVector MuzzleOffset;

	float m_Turn_Rate;
	float m_Look_Rate;
	bool is_Dashing;
	bool can_Cast;

	// Timer handle for handling dash function
	FTimerHandle DashTimerHandle;
	// Timer handle to handle projectile cooldown
	FTimerHandle ProjectileTimerHandle;

	//Spring Arm Component for controlling the camera
	UPROPERTY(VisibleDefaultsOnly)
		USpringArmComponent* CameraBoom;

	//Camera Component
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
		UCameraComponent* Camera;


	// Health
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float m_Player_Health_Points = 1.f;
	float m_Max_Health = 1.f;

	// Energy
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float m_Player_Energy_Points = 1.f;
	float m_Max_Energy = 1.f;

	// Experience
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float m_Player_Experience_Points = 0.f;
	float m_Max_Experience = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FRotator m_Cam_Rotate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool m_Can_Move = true;

	//Perspective
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float m_Rotation_Angle = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float m_Target_Angle = 45.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float m_Rotation_Speed = 200.f;

};