// Fill out your copyright notice in the Description page of Project Settings.

#include "AoeAttackController.h"
#include "Engine/Engine.h"
#include "Engine/DecalActor.h"
#include "Components/DecalComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

// Sets default values
AAoeAttackController::AAoeAttackController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Collision Component Set Up
	if (!CollisionComponent)
	{
		// Set Collision box to be sphere.
		CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
		// Set collision box radius.
		CollisionComponent->InitSphereRadius(500.0f);
		// Set the root component to be newly created component.
		RootComponent = CollisionComponent;
	}

	// Projectile Movement Component Set Up
	if (!ProjectileMovementComponent)
	{
		// Use this component to drive projectile movement.
		ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
		ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
		ProjectileMovementComponent->InitialSpeed = 0.0f;
		ProjectileMovementComponent->MaxSpeed = 0.0f;
		ProjectileMovementComponent->bRotationFollowsVelocity = true;
		ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	}

	// Projectile Mesh Set Up


	// Decal Set Up
	if (!AOEIndicatorComponent)
	{
		// sets mesh of projectile to basic sphere mesh, loaded from unreal engine files
		AOEIndicatorComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComponent"));
		static ConstructorHelpers::FObjectFinder<UMaterial>DecalMaterial(TEXT("Material'/Game/Fruitcake_Game/Enemies/EnemyAssets/Mushroom/AOEDecal.AOEDecal'"));
		if (DecalMaterial.Succeeded())
		{
			// Set Decal Material
			AOEIndicatorComponent->SetDecalMaterial(DecalMaterial.Object);
		}
		// set how long projectile will last in seconds, after this amount of time, projectile is destroyed
		AOEIndicatorComponent->DecalSize = FVector(400.0f, 400.0f, 400.0f);
		AOEIndicatorComponent->SetupAttachment(RootComponent);
	}

	// Load material for projectile from unreal files
	static ConstructorHelpers::FObjectFinder<UMaterial>Material(TEXT("Material'/Game/Fruitcake_Game/Materials/Red.Red'"));
	if (Material.Succeeded())
	{
	//	ProjectileMaterialInstance = UMaterialInstanceDynamic::Create(Material.Object, ProjectileMeshComponent);
	}

	static ConstructorHelpers::FObjectFinder<UMaterial>BlueMaterial(TEXT("Material'/Game/Fruitcake_Game/Materials/Blue.Blue'"));
	if (BlueMaterial.Succeeded())
	{
	//	BlueMaterialInstance = UMaterialInstanceDynamic::Create(BlueMaterial.Object, ProjectileMeshComponent);
	}


	// Event called when component hits something.
	CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("EnemyAOE"));


}

// Called when the game starts or when spawned
void AAoeAttackController::BeginPlay()
{
	Super::BeginPlay();

	// Store Player Character in the pointer
	PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	// AOE Activation Timer - CHANGE NEEDED (Need to make the time customisable for different types of attacks)
	GetWorldTimerManager().SetTimer(AOEActivateTimer, this, &AAoeAttackController::ActivateAOE, 2.4f, false);

}


void AAoeAttackController::FireAtLocation(const FVector& SpawnLoaction, const float& AoeSize)
{
	// Sets velocity vector to be the direction multipled by the initial speed of the projectile

	// SpawnLocation = AOE Spawn Location, to make AOE spawn on ground, spawn at ActorLocation - 90
	CollisionComponent->SetSphereRadius(AoeSize * 50);

	ProjectileMovementComponent->Velocity = SpawnLoaction * ProjectileMovementComponent->InitialSpeed;
}

void AAoeAttackController::GetTarget()
{

}

void AAoeAttackController::ActivateAOE()
{
	if (CollisionComponent->IsOverlappingActor(PlayerCharacter))
	{
		PlayerCharacter->TakeDamage(10.f, FDamageEvent(), nullptr, this);
	}
}


