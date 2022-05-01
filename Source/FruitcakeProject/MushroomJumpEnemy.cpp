// Fill out your copyright notice in the Description page of Project Settings.

#include "Kismet/GameplayStatics.h"
#include "Engine/TimelineTemplate.h"
#include "Components/CapsuleComponent.h"
#include "AoeAttackController.h"
#include "Components/SphereComponent.h"
#include "MushroomJumpEnemy.h"

// Sets default values
AMushroomJumpEnemy::AMushroomJumpEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Get capsule component
	MushroomCapsule = GetCapsuleComponent();
	MushroomCapsule->SetCollisionProfileName(TEXT("Enemy"));
	
	// Create Mushroom Spline Component
	if(!MushroomJumpSpline)
	{
		MushroomJumpSpline = CreateDefaultSubobject<USplineComponent>(TEXT("MushroomJumpSpline"));
		MushroomJumpSpline->SetupAttachment(RootComponent);
	}

	// Create Jump Timeline
	if(!JumpTimelineComponent)
	{
		JumpTimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("JumpTimeline"));
	}

	// Create Detection Sphere Component
	if(!DetectionSphere)
	{
		DetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionSphere"));
		DetectionSphere->SetupAttachment(RootComponent);
	}
}

// Called when the game starts or when spawned
void AMushroomJumpEnemy::BeginPlay()
{
	Super::BeginPlay();
	ResetJump();
	MushroomJumpSpline->Duration = 1.f;
	

	Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	JumpTimelineComponent->SetPlayRate(JumpSpeed);
	
	// Set up Jump Timer Handle
	GetWorldTimerManager().SetTimer(JumpTimerHandle, this, &AMushroomJumpEnemy::JumpTowardsPlayer, 5.f, true);
	
}

// Called every frame
void AMushroomJumpEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMushroomJumpEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

bool AMushroomJumpEnemy::CalculateJump()
{
	// Set JumpTo to player character location
	JumpTo = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetActorLocation();
	
	JumpDistance = FVector::Dist(GetActorLocation(), JumpTo);

	// Check if jump distance is in between min and max
	if (JumpDistance > MinimumJumpDistance && JumpDistance < MaximumJumpDistance)
	{
		bReadyToJump = true;
	}
	else
	{
		bReadyToJump = false;
	}

	if(!bReadyToJump)
	{
		ResetJump();
		return false;
	}

	JumpFrom = GetActorLocation();

	/* Set Spline Points */

	// Spline Point One
	SplinePoints.Add(JumpFrom);
	
	// Spline Point Two
	FVector Midpoint = (JumpFrom + JumpTo) / 2;
	Midpoint += FVector(0, 0, JumpHeight);
	SplinePoints.Add(Midpoint);
	
	// Spline Point Three
	SplinePoints.Add(JumpTo);

	// Set Spline
	MushroomJumpSpline->SetSplinePoints(SplinePoints, ESplineCoordinateSpace::World, true);

	// Set Tangent at Spline Point Two

	// Clamp Jump Distance
	JumpDistance = FMath::Clamp(JumpDistance, MinimumJumpDistance, MaximumJumpDistance);
	const FVector Tangent = JumpDistance * FVector(0.f, 0.f, -1.f);
	MushroomJumpSpline->SetTangentsAtSplinePoint(2, Tangent, Tangent, ESplineCoordinateSpace::World, true);

	
	
	return true;
}



void AMushroomJumpEnemy::JumpTowardsPlayer()
{
	// Check if overlapping player
	if (!DetectionSphere->IsOverlappingActor(Player))
	{
		return;
	}

	if (bIsJumping)
	{
		return;
	}

	if(!CalculateJump())
	{
		return;
	}

	// Spawn AOE
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;
	GetWorld()->SpawnActor<AAoeAttackController>(MushroonmAoeAttackController, GetActorLocation(), GetActorRotation(), SpawnParams);

	bReadyToJump = false;
	bIsJumping = true;
	Jump();

	// Play jump timeline
	JumpTimelineComponent->PlayFromStart();
	
	
}

void AMushroomJumpEnemy::ResetJump()
{
	JumpTo = FVector(0, 0, 0);
	JumpFrom = FVector(0, 0, 0);
	JumpDistance = 0;
	bReadyToJump = false;
	bIsJumping = false;
}


