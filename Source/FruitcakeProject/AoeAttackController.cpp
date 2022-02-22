// Fill out your copyright notice in the Description page of Project Settings.

#include "AoeAttackController.h"
#include "Engine/Engine.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

// Sets default values
AAoeAttackController::AAoeAttackController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!CollisionComponent)
	{
		// Set Collsion box to be sphere.
		CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
		// Set collision box radius.
		CollisionComponent->InitSphereRadius(500.0f);
		// Set the root component to be newly created component.
		RootComponent = CollisionComponent;
	}

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

	if (!ProjectileMeshComponent)
	{
		// sets mesh of projectile to basic sphere mesh, loaded from unreal engine files
		ProjectileMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComponent"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Cylinder.Shape_Cylinder'"));
		if (Mesh.Succeeded())
		{
			ProjectileMeshComponent->SetStaticMesh(Mesh.Object);
		}
		// set how long projectile will last in seconds, after this amount of time, projectile is destroyed
		InitialLifeSpan = 2.6f;
	}

	// Load material for projectile from unreal files
	static ConstructorHelpers::FObjectFinder<UMaterial>Material(TEXT("Material'/Game/Fruitcake_Game/Materials/Red.Red'"));
	if (Material.Succeeded())
	{
		ProjectileMaterialInstance = UMaterialInstanceDynamic::Create(Material.Object, ProjectileMeshComponent);
	}

	static ConstructorHelpers::FObjectFinder<UMaterial>BlueMaterial(TEXT("Material'/Game/Fruitcake_Game/Materials/Blue.Blue'"));
	if (BlueMaterial.Succeeded())
	{
		BlueMaterialInstance = UMaterialInstanceDynamic::Create(BlueMaterial.Object, ProjectileMeshComponent);
	}
	ProjectileMeshComponent->SetMaterial(0, ProjectileMaterialInstance);

	ProjectileMeshComponent->BodyInstance.SetCollisionProfileName(TEXT("EnemyAOE"));

	// Set size of projectile
	ProjectileMeshComponent->SetRelativeScale3D(FVector(10.f, 10.f, .05f));
	ProjectileMeshComponent->SetupAttachment(RootComponent);

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

// Called every frame
void AAoeAttackController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AAoeAttackController::FireAtLocation(const FVector& SpawnLoaction, const float& AoeSize)
{
	// Sets velocity vector to be the direction multipled by the initial speed of the projectile

	// SpawnLocation = AOE Spawn Location, to make AOE spawn on ground, spawn at ActorLocation - 90
	CollisionComponent->SetSphereRadius(AoeSize * 50);
	ProjectileMeshComponent->SetRelativeScale3D(FVector(AoeSize, AoeSize, .05f));

	ProjectileMovementComponent->Velocity = SpawnLoaction * ProjectileMovementComponent->InitialSpeed;
}

void AAoeAttackController::GetTarget()
{

}

void AAoeAttackController::ActivateAOE()
{
	ProjectileMeshComponent->SetMaterial(0, BlueMaterialInstance);

	if (CollisionComponent->IsOverlappingActor(PlayerCharacter))
	{
		PlayerCharacter->ReducePlayerHealth();
	}
}


