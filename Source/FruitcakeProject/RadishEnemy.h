// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/MovementComponent.h"

#include "RadishEnemy.generated.h"

UCLASS(Blueprintable)
class FRUITCAKEPROJECT_API ARadishEnemy : public APawn
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARadishEnemy();

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

	UFUNCTION()
		void OnTriggerBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnTriggerEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
		void SetStunned();
protected:

	// pointer to player character
	class APlayerCharacter* PlayerCharacter;

	FTimerHandle AttackTimerHandle;
	FTimerHandle AoeAttackTimerHandle;
	FTimerHandle StunTimerHandle;

	//Sphere collision component.
	UPROPERTY(VisibleDefaultsOnly, Category = "RadishEnemy")
		UBoxComponent* CollisionComponent;
	 
	// Cylinder Body Static Mesh
	UPROPERTY(VisibleDefaultsOnly, Category = "RadishEnemy")
		UStaticMeshComponent* RadishEnemyMeshComponent;

	//Sphere Weak Point Static Mesh
	UPROPERTY(VisibleDefaultsOnly, Category = "RadishEnemy")
		UStaticMeshComponent* WeakPointMeshComponent;

	// Sphere range detection
	UPROPERTY(VisibleDefaultsOnly, Category = "RadishEnemy")
		USphereComponent* SightSphere;

	UPROPERTY(BlueprintReadWrite, Category = "RadishEnemy")
		bool bHostile;

	UPROPERTY(BlueprintReadWrite, Category = "RadishEnemy")
		bool bStunned;

	UPROPERTY(BlueprintReadWrite, Category = "RadishEnemy")
		float MovementSpeed;

	// Radish projectiles spawn class (Vector)
	UPROPERTY(EditDefaultsOnly, Category = "RadishEnemy")
		TSubclassOf<class AProjectiles> RadishProjectiles;

	// AOE attack spawn class (Vector)
	UPROPERTY(EditDefaultsOnly, Category = "RadishEnemy")
		TSubclassOf<class AAoeAttackController> RadishAoeAttacks;

	// Gun muzzle offset from the camera location
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RadishEnemy")
		FVector MuzzleOffset;

	//test mats
	UPROPERTY(VisibleDefaultsOnly, Category = "RadishEnemy")
		UMaterial* default_material;

	UPROPERTY(VisibleDefaultsOnly, Category = "RadishEnemy")
		UMaterial* red_material;
};
