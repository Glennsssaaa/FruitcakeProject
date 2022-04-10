// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Projectiles.generated.h"

UCLASS()
class FRUITCAKEPROJECT_API AProjectiles : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AProjectiles();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// initial firing function
	void FireInDirection(const FVector& ShootDirection, bool isHoming, bool isPlayer);

	// called after initial function if bullet is set to homing, controls homing
	void HomingOnTarget();

	// gets projectiles target if is set to homing
	void GetTarget();

	void EditTweet();

	// Function that is called when the projectile hits something.
	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

protected:
	FTimerHandle ProjectileTimerHandle;

	class APlayerCharacter* PlayerCharacter;

	// Sphere Static Mesh
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
		UStaticMeshComponent* ProjectileMeshComponent;

	// Projectile material
	UPROPERTY(VisibleDefaultsOnly, Category = Movement)
		UMaterialInstanceDynamic* ProjectileMaterialInstance;

	// Sphere collision component.
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
		USphereComponent* CollisionComponent;

	// Projectile movement component.
	UPROPERTY(VisibleAnywhere, Category = Movement)
		UProjectileMovementComponent* ProjectileMovementComponent;

	bool isPlayerProjectile;
};