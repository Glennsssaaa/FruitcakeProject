// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"

#include "Components/CapsuleComponent.h"

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

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);


	// Weak Point Collision Functions 
	//UFUNCTION()
	//	void OnWeakPointOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Sight Sphere Collision Functions
	UFUNCTION()
		void OnTriggerBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnTriggerEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// Attack Range Collision Functions
	UFUNCTION()
		void OnAttackRangeOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnAttackRangeOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
		void ReducePlayerHealth();

	UFUNCTION()
		void SetStunned();

	UFUNCTION(BlueprintCallable, Category = "C++ Functions")
		bool CheckForOverlapMethod();

	void CheckIfStillOverlapping();
	void SetAttackDelayBool();
	void RotateTowardsPlayer();
protected:

	// pointer to player character
	class APlayerCharacter* PlayerCharacter;

	FTimerHandle AttackTimerHandle;
	FTimerHandle AttackDelayTimerHandle;
	FTimerHandle StunTimerHandle;


	// Sphere collision component.
	UPROPERTY(VisibleDefaultsOnly, Category = "RadishEnemy")
		UBoxComponent* CollisionComponent;

	//// Sphere Weak Point Static Mesh
	//UPROPERTY(VisibleDefaultsOnly, Category = "RadishEnemy")
	//	UStaticMeshComponent* WeakPointMeshComponent;

	// Sphere range detection
	UPROPERTY(VisibleDefaultsOnly, Category = "RadishEnemy")
		USphereComponent* SightSphere;

	// Attack Range Collision Component
	UPROPERTY(BlueprintReadWrite, Category = "RadishEnemy")
		USphereComponent* AttackRange;

	// Sets if enemy is moving towards player or not
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RadishEnemy")
		bool bHostile;

	// Sets if enemy is stunned or not
	UPROPERTY(BlueprintReadWrite, Category = "RadishEnemy")
		bool bStunned;

	// Sets if enemy is attacking or not
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RadishEnemy")
		bool bAttack;

	UPROPERTY(BlueprintReadWrite, Category = "RadishEnemy")
		float MovementSpeed;

	//test mats
	UPROPERTY(VisibleDefaultsOnly, Category = "RadishEnemy")
		UMaterial* default_material;

	UPROPERTY(VisibleDefaultsOnly, Category = "RadishEnemy")
		UMaterial* red_material;

	//death boolean
	UPROPERTY(BlueprintReadWrite)
	bool dead;

	//death timer
	UPROPERTY(BlueprintReadWrite)
	float deathTimer;
	
	int health_pool;
	UPROPERTY(BlueprintReadWrite, Category = "RadishEnemy")
		bool b_attack_delay_active;

	// Attack Time Delay, editable in blueprint
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RadishEnemy")
		float f_attack_delay_time;

};