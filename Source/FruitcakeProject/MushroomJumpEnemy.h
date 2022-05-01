// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Character.h"
#include "MushroomJumpEnemy.generated.h"

UCLASS()
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

	bool CalculateJump();
	void ResetJump();
	void JumpTowardsPlayer();
	
protected:

	// Spline jump variables

#pragma region Jump Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MushroomEnemy")
	FVector JumpTo;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MushroomEnemy")
	FVector JumpFrom;
	
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

	
	// Set Up Timeline
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline")
	class UTimelineComponent* JumpTimelineComponent;

	// Detection Sphere Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MushroomEnemy")
	class USphereComponent* DetectionSphere;

	// Mushroom Capsule Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MushroomEnemy")
	class UCapsuleComponent* MushroomCapsule;

	// Mushroom Jump Timer Handle
	FTimerHandle JumpTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MushroomEnemy")
	USplineComponent* MushroomJumpSpline;

	// Flower projectiles spawn class (Vector)
	UPROPERTY()
	TSubclassOf<class AAoeAttackController> MushroonmAoeAttackController;
	
	UPROPERTY()
	AActor* Player;
};
