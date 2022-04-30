// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

	void CalculateJump();
	
protected:

	// Spline jump variables
	FVector JumpTo;
	FVector JumpFrom;
	
	float JumpDistance;
	float JumpHeight;
	float JumpSpeed;
	float MinimumJumpDistance;
	float MaximumJumpDistance;
	
	bool bIsJumping;
	bool bReadyToJump;
	bool bIsLandingVertical;
	


};
