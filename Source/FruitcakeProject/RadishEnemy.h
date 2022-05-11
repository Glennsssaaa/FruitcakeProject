// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	// Sight Sphere Collision Functions
	UFUNCTION()
	void OnTriggerBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnTriggerEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// Attack Range Collision Functions
	UFUNCTION()
	void OnAttackRangeOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void ReducePlayerHealth();

	UFUNCTION()
	void SetStunned();
	
	UFUNCTION(BlueprintCallable, Category = "Destroy")
	void UnbindDelegatesAndDestroy();

	void CheckIfStillOverlapping();
	void SetAttackDelayBool();
	void RotateTowardsPlayer();
protected:

	UPROPERTY()
	class APlayerCharacter* Player;
	
	// Sphere collision component.
	UPROPERTY(BlueprintReadWrite, Category = "RadishEnemy")
	UBoxComponent* CollisionComponent;

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

	// Set to true if enemy has been damaged, used in animation bp
	UPROPERTY(BlueprintReadWrite, Category = "RadishEnemy")
	bool bJustDamaged;
	
	UPROPERTY(BlueprintReadWrite, Category = "RadishEnemy")
	float MovementSpeed;
	
	// Death bool
	UPROPERTY(BlueprintReadWrite)
	bool bDead;

	// Death timer
	UPROPERTY(BlueprintReadWrite)
	float DeathTimer;
	
	UPROPERTY(BlueprintReadWrite, Category = "RadishEnemy")
	bool bAttackDelayActive;

	// Attack Time Delay, editable in blueprint
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RadishEnemy")
	float AttackDelayTime;

	FTimerHandle AttackTimerHandle;
	FTimerHandle AttackDelayTimerHandle;
	FTimerHandle StunTimerHandle;
	
};