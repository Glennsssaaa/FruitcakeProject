// Fill out your copyright notice in the Description page of Project Settings.

#include "Kismet/GameplayStatics.h"
#include "Engine/TimelineTemplate.h"
#include "Components/CapsuleComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SphereComponent.h"
#include "MushroomJumpEnemy.h"

// Sets default values
AMushroomJumpEnemy::AMushroomJumpEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Get capsule component
	if(!MushroomCapsule)
	{
		MushroomCapsule = GetCapsuleComponent();
		MushroomCapsule->SetCollisionProfileName(TEXT("Enemy"));
	}



	MushroomJumpSpline = CreateDefaultSubobject<USplineComponent>(TEXT("MushroomJumpSpline"));
	MushroomJumpSpline->SetupAttachment(RootComponent);
	MushroomJumpSpline->SetAbsolute(true, true, true);
	// Set up mushroom spline component
	MushroomJumpSpline->SetVisibility(true, true);


	// Add overlap function to capsule
	MushroomCapsule->OnComponentBeginOverlap.AddDynamic(this, &AMushroomJumpEnemy::OnOverlapBegin);
	

	// Create Detection Sphere Component
	if(!DetectionSphere)
	{
		DetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionSphere"));
		DetectionSphere->InitSphereRadius(1200.f);
		DetectionSphere->SetCollisionProfileName(TEXT("AICollision"));
		DetectionSphere->SetupAttachment(RootComponent);
	}


}

// Called when the game starts or when spawned
void AMushroomJumpEnemy::BeginPlay()
{
	Super::BeginPlay();
	ResetJumpNew();





	
	bIsJumping = false;
	Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	MushroomAoeAttackController = AAoeAttackController::StaticClass();

	bIsJumping = true;
	bReadyToJump = true;
	bIsLandingVertical = true;
	MinimumJumpDistance = 500.f;
	MaximumJumpDistance = 5000.f;
	JumpSpeed = 1.25f;
	JumpHeight = 0.35f;
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

bool AMushroomJumpEnemy::CalculateJumpNew()
{
	// Set JumpTo to player character location
	JumpTo = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetActorLocation();
	
	JumpDistance = FVector::Dist(JumpTo, GetActorLocation());

	// Check if jump distance is in between min and max
	if (JumpDistance >= MinimumJumpDistance && JumpDistance <= MaximumJumpDistance)
	{
		bReadyToJump = true;
	}
	else
	{
		bReadyToJump = false;
	}

	if(!bReadyToJump)
	{
		ResetJumpNew();
		return false;
	}

	JumpFrom = GetActorLocation();

	/* Set Spline Points */
	
	// Spline Point Two
	FVector Midpoint = (JumpFrom + JumpTo) / 2;
	Midpoint += FVector(0, 0, JumpHeight * JumpDistance);
	
	// set mushroom jump spline points
	
	
	// Set Spline

	// Make Array
	TArray<FVector> SplinePointsArray;
	SplinePointsArray.Add(JumpFrom);
	SplinePointsArray.Add(Midpoint);
	SplinePointsArray.Add(JumpTo);
	
	
	MushroomJumpSpline->SetSplinePoints(SplinePointsArray, ESplineCoordinateSpace::World, true);

	
	// Set Tangent at Spline Point Two

	// Clamp Jump Distance
	JumpDistance = FMath::Clamp(JumpDistance, 0.f, 2000.f);
	const FVector Tangent = JumpDistance * FVector(0.f, 0.f, -1.f);
	MushroomJumpSpline->SetTangentsAtSplinePoint(2, Tangent, Tangent, ESplineCoordinateSpace::World, true);

	
	return true;
}



bool AMushroomJumpEnemy::JumpTowardsPlayerNew()
{
	// Check if overlapping player
	// if (!DetectionSphere->IsOverlappingActor( UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	// {
	// //	return false;
	// }
	//
	// if (bIsJumping)
	// {
	// //	return false;
	// }
	//
	//
	//
	// bIsJumping = false;


	
	if(!CalculateJumpNew())
	{
		return false;
	}


	bIsJumping = false;
	
	// Spawn AOE
	FActorSpawnParameters SpawnParams = FActorSpawnParameters();
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();
	FVector SpawnLocation = FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z - 80.f);
	GetWorld()->SpawnActor<AAoeAttackController>(MushroomAoeAttackController, UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetActorLocation(), FRotator(0,0,0), SpawnParams);
	
	bReadyToJump = false;
	bIsJumping = true;
	Jump();

	return true;
}



void AMushroomJumpEnemy::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!OtherActor->IsA(APlayerCharacter::StaticClass()))
	{
		return;
	}

	if(OverlappedComp->GetCollisionProfileName() == "PlayerAttack")
	{
		TakeDamage(1.f, FDamageEvent(), nullptr, this);
	}
}

void AMushroomJumpEnemy::ResetJumpNew()
{
	JumpTo = FVector(0, 0, 0);
	JumpFrom = FVector(0, 0, 0);
	JumpDistance = 0;
	bReadyToJump = false;
	bIsJumping = false;
}


