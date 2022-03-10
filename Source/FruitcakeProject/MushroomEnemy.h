// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/BoxComponent.h"

#include "MushroomEnemy.generated.h"

UCLASS(Blueprintable)
class FRUITCAKEPROJECT_API AMushroomEnemy : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMushroomEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void FireAtPlayer();

	void FireAoeAtPlayer();

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:

	// pointer to player character
	class APlayerCharacter* PlayerCharacter;

	FTimerHandle AttackTimerHandle;
	FTimerHandle AoeAttackTimerHandle;

	// Sphere collision component.
	UPROPERTY(VisibleDefaultsOnly, Category = "MushroomEnemy")
		UBoxComponent* CollisionComponent;

	// Sphere Static Mesh
	UPROPERTY(VisibleDefaultsOnly, Category = "MushroomEnemy")
		UStaticMeshComponent* MushroomEnemyMeshComponent;

	// Sphere range detection
	/*UPROPERTY(VisibleDefaultsOnly, Category = "RadishEnemy")
		USphereComponent* SightSphere;*/

	// Mushroom projectiles spawn class (Vector)
	UPROPERTY(EditDefaultsOnly, Category = "MushroomEnemy")
		TSubclassOf<class AProjectiles> MushroomProjectiles;

	// AOE attack spawn class (Vector)
	UPROPERTY(EditDefaultsOnly, Category = "MushroomEnemy")
		TSubclassOf<class AAoeAttackController> MushroomAoeAttacks;

	// Gun muzzle offset from the camera location
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MushroomEnemy")
		FVector MuzzleOffset;

};
