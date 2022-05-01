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
	void ActivateAoe();

	
protected:

	// AOE Collision Sphere Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* AoeCollisionSphere;

	// AOE Timer Handle
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components");
	FTimerHandle AoeTimerHandle;

	// AOE Decal
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UDecalComponent* AoeDecal;

	// AOE Decal Material
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UMaterialInterface* AoeDecalMaterial;

	float AoeLifeSpan;
};
