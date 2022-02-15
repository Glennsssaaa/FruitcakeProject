// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"

#include "FlowerEnemy.generated.h"

UCLASS(Blueprintable)
class FRUITCAKEPROJECT_API AFlowerEnemy : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFlowerEnemy();

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

	// Sphere collision component.
	UPROPERTY(VisibleDefaultsOnly, Category = "FlowerEnemy")
		UBoxComponent* CollisionComponent;

	// Sphere Static Mesh
	UPROPERTY(VisibleDefaultsOnly, Category = "FlowerEnemy")
		UStaticMeshComponent* FlowerEnemyMeshComponent;

	// Flower projectiles spawn class (Vector)
	UPROPERTY(EditDefaultsOnly, Category = "FlowerEnemy")
		TSubclassOf<class AProjectiles> FlowerProjectiles;

	// AOE attack spawn class (Vector)
	UPROPERTY(EditDefaultsOnly, Category = "FlowerEnemy")
		TSubclassOf<class AAoeAttackController> FlowerAoeAttacks;

	// Gun muzzle offset from the camera location
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FlowerEnemy")
		FVector MuzzleOffset;
};
