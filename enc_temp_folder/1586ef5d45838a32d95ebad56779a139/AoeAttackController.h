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
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Fire Aoe Attack at specific location
	void FireAtLocation(const FVector& SpawnLoaction, const float& AoeSize);

	// Get Target Enemy (Maybe not necessary)
	void GetTarget();

	void ActivateAOE();

	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
protected:

	// Pointer to player character, used to access their location at all times, which is used for targetting them with AOE attacks
	bool isPlayerHit = false;
	FTimerHandle AOEActivateTimer;

	class APlayerCharacter* PlayerCharacter;

	// Sphere Static Mesh
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
		UStaticMeshComponent* ProjectileMeshComponent;

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
