// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/PointLightComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
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
	UFUNCTION()
	void FireInDirection(const FVector& ShootDirection, bool isHoming, bool isPlayer);

	// called after initial function if bullet is set to homing, controls homing
	void HomingOnTarget() const;

	// gets projectiles target if is set to homing
	static void GetTarget();

	// Function that is called when the projectile hits something.
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void Kill();
	
protected:
	FTimerHandle ProjectileTimerHandle;
	FTimerHandle KillTimerHandle;

	UPROPERTY()
	class APlayerCharacter* PlayerCharacter;

	// Sphere Static Mesh
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	UStaticMeshComponent* ProjectileMeshComponent;

	// Projectile materials
	UPROPERTY(VisibleDefaultsOnly, Category = Movement)
	UMaterialInstanceDynamic* ProjectileMaterialInstancePlayer;

	UPROPERTY(VisibleDefaultsOnly, Category = Movement)
	UMaterialInstanceDynamic* ProjectileMaterialInstanceEnemy;

	// Sphere collision component.
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	USphereComponent* CollisionComponent;

	// Projectile movement component.
	UPROPERTY(VisibleAnywhere, Category = Movement)
	UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(VisibleAnywhere)
	UNiagaraSystem* ProjectileParticleEffectN;

	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* ParticleComponent;

	//Point Light
	UPROPERTY(VisibleAnywhere)
	UPointLightComponent* PointLightComponent;

	bool isPlayerProjectile; 
};