// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/BoxComponent.h"

#include "FlowerEnemy.generated.h"

UCLASS(Blueprintable)
class FRUITCAKEPROJECT_API AFlowerEnemy : public APawn
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

	void CallFireAtPlayer();
	
	UFUNCTION(BlueprintCallable, Category = "EnemyAttack")
	void FireAtPlayer();



	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
protected:

	// pointer to player character
	UPROPERTY()
	class APlayerCharacter* PlayerCharacter;

	FTimerHandle AttackTimerHandle;
	FTimerHandle AoeAttackTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsShooting;
	
	// Sphere collision component.
	UPROPERTY(VisibleDefaultsOnly, Category = "FlowerEnemy")
	UBoxComponent* CollisionComponent;

	// Sphere collision component.
	UPROPERTY(VisibleDefaultsOnly, Category = "FlowerEnemy")
	UBoxComponent* ProjectileSpawnPoint;

	// Flower projectiles spawn class (Vector)
	UPROPERTY(EditDefaultsOnly, Category = "FlowerEnemy")
	TSubclassOf<class AProjectiles> FlowerProjectiles;

	// Gun muzzle offset from the camera location
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FlowerEnemy")
	FVector MuzzleOffset;
};
