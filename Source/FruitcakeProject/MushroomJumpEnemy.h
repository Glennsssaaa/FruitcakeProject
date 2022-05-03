// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "AoeAttackController.h"
#include "GameFramework/Character.h"
#include "MushroomJumpEnemy.generated.h"

UCLASS(Blueprintable)
class FRUITCAKEPROJECT_API AMushroomJumpEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMushroomJumpEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	UFUNCTION(BlueprintCallable, Category = "EnemyAttack")
	bool CalculateJumpNew();
	UFUNCTION(BlueprintCallable, Category = "EnemyAttack")
	void ResetJumpNew();
	UFUNCTION(BlueprintCallable, Category = "EnemyAttack")
	bool JumpTowardsPlayerNew();


	

	
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
protected:

	// Spline jump variables

#pragma region Jump Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MushroomEnemy")
	FVector JumpTo;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MushroomEnemy")
	FVector JumpFrom;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MushroomEnemy")
	FVector BaseLocation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MushroomEnemy")
	float JumpDistance;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MushroomEnemy")
	float JumpHeight;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MushroomEnemy")
	float JumpSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MushroomEnemy")
	float MinimumJumpDistance;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MushroomEnemy")
	float MaximumJumpDistance;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MushroomEnemy")
	TArray<FVector> SplinePoints;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MushroomEnemy")
	bool bIsJumping;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MushroomEnemy")
	bool bReadyToJump;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MushroomEnemy")
	bool bIsLandingVertical;

#pragma endregion
	
	
	// Detection Sphere Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MushroomEnemy")
	class USphereComponent* DetectionSphere;

	// Mushroom Capsule Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MushroomEnemy")
	class UCapsuleComponent* MushroomCapsule;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MushroomEnemy")
	class USplineComponent* MushroomJumpSpline;

	// Flower projectiles spawn class (Vector)
	UPROPERTY(EditDefaultsOnly, Category = "MushroomEnemy")
	TSubclassOf<class AAoeAttackController> MushroomAoeAttackController;
	
	UPROPERTY()
	AActor* Player;
};
