// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"

#include "RadishEnemy.generated.h"

UCLASS()
class FRUITCAKEPROJECT_API ARadishEnemy : public AActor
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
protected:

	// pointer to player character
	class APlayerCharacter* PlayerCharacter;

	FTimerHandle AttackTimerHandle;
	FTimerHandle AoeAttackTimerHandle;

	//Sphere collision component.
	UPROPERTY(VisibleDefaultsOnly, Category = "RadishEnemy")
		UBoxComponent* CollisionComponent;
	 
	// Sphere Static Mesh
	UPROPERTY(VisibleDefaultsOnly, Category = "RadishEnemy")
		UStaticMeshComponent* RadishEnemyMeshComponent;

	// Radish projectiles spawn class (Vector)
	UPROPERTY(EditDefaultsOnly, Category = "RadishEnemy")
		TSubclassOf<class AProjectiles> RadishProjectiles;

	// AOE attack spawn class (Vector)
	UPROPERTY(EditDefaultsOnly, Category = "RadishEnemy")
		TSubclassOf<class AAoeAttackController> RadishAoeAttacks;

	// Gun muzzle offset from the camera location
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RadishEnemy")
		FVector MuzzleOffset;

};
