// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"

#include "Components/DecalComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "AoeAttackController.generated.h"


#pragma region 	STEPS TO INCLUDE AOE ATTACKS IN ANOTHER CLASS - DROPDOWN

/*
		STEPS TO INCLUDE AOE ATTACKS IN ANOTHER CLASS

	1. include file in .cpp file of class - #include "AoeAttackController.h"
	2. declare in header file - TSubclassOf<class AAoeAttackController> objectName;
	3. initialise in beginplay function - objectName = AAoeAttackController::StaticClass();
	4. create function for firing attacks

	EXAMPLE FUNCTION: 

	void AFlowerEnemy::FireAoeAtPlayer()
	{
		if (FlowerAoeAttacks)
		{
			FVector SpawnLocation;
			FRotator CameraRotation;
			SpawnLocation = FVector(PlayerCharacter->GetActorLocation().X, PlayerCharacter->GetActorLocation().Y, PlayerCharacter->GetActorLocation().Z - 90.f);

			// set rotation of projectile to camera rotation
			FRotator MuzzleRotation = CameraRotation;

			UWorld* World = GetWorld();
			if (World)
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = this;
				SpawnParams.Instigator = GetInstigator();

				// Spawn the projectile at the muzzle.
				AAoeAttackController* Projectile = World->SpawnActor<AAoeAttackController>(FlowerAoeAttacks, SpawnLocation, FRotator(0.f, 0.f, 0.f), SpawnParams);

				if (Projectile)
				{
					// Set the projectile's initial trajectory.
					FVector LaunchDirection = MuzzleRotation.Vector();
					Projectile->FireAtLocation(LaunchDirection, 3.f);
				}

			}
		}
	}
*/

#pragma endregion 




UCLASS()
class FRUITCAKEPROJECT_API AAoeAttackController : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAoeAttackController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:


	// Fire Aoe Attack at specific location
	void FireAtLocation(const FVector& SpawnLocation, const float& AoeSize);

	// Get Target Enemy (Maybe not necessary)
	void GetTarget();

	void ActivateAOE();
protected:

	// Pointer to player character, used to access their location at all times, which is used for targetting them with AOE attacks
	bool isPlayerHit = false;
	FTimerHandle AOEActivateTimer;

	class APlayerCharacter* PlayerCharacter;



	// AOE Indicator Decal
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	UDecalComponent* AOEIndicatorComponent;
	
	// Projectile material
	UPROPERTY(VisibleDefaultsOnly, Category = Movement)
		UMaterialInstanceDynamic* ProjectileMaterialInstance;

	// Projectile material
	UPROPERTY(VisibleDefaultsOnly, Category = Movement)
		UMaterialInstanceDynamic* BlueMaterialInstance;

	//UMaterialInterface* Material;

	// Sphere collision component.
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
		USphereComponent* CollisionComponent;

	// Box collision component.
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
		UBoxComponent* BoxCollisionComponent;
	// Decal Component
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
		UDecalComponent* AOECircleComponent;

	// Projectile movement component.
	UPROPERTY(VisibleAnywhere, Category = Movement)
		UProjectileMovementComponent* ProjectileMovementComponent;
};
