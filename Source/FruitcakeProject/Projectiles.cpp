// Fill out your copyright notice in the Description page of Project Settings.

#include "Projectiles.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "PlayerCharacter.h"
#include "Engine/Engine.h"


// Sets default values
AProjectiles::AProjectiles()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// collision component set up
	if (!CollisionComponent)
	{
		// Set Collsion box to be sphere.
		CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
		CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Nothing"));

		// Set collision box radius.
		CollisionComponent->InitSphereRadius(15.0f);
		// Set the root component to be newly created component.
		RootComponent = CollisionComponent;
	}

	// projectile movement component set up
	if (!ProjectileMovementComponent)
	{
		// Use this component to drive projectile movement.
		ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));

		// assign the collision component to movement component
		ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);

		// set up projecitle initial and maximum speeds
		ProjectileMovementComponent->InitialSpeed = 500.0f;
		ProjectileMovementComponent->MaxSpeed = 500.0f;

		ProjectileMovementComponent->bRotationFollowsVelocity = true;
		ProjectileMovementComponent->ProjectileGravityScale = 0.0f;

		// set if projectile is homing or not
		ProjectileMovementComponent->bIsHomingProjectile = true;
		// set how quickly projectile will home in
		ProjectileMovementComponent->HomingAccelerationMagnitude = 3000.f;
	}
	// mesh component set up
	if (!ProjectileMeshComponent)
	{
		// sets mesh of projectile to basic sphere mesh, loaded from unreal engine files
		ProjectileMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComponent"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere'"));
		if (Mesh.Succeeded())
		{
			ProjectileMeshComponent->SetStaticMesh(Mesh.Object);
		}
		// set how long projectile will last in seconds, after this amount of time, projectile is destroyed
		InitialLifeSpan = 3.f;
	}
	//point light set up
	if (!PointLightComponent)
	{
		PointLightComponent = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLightComponent"));

		PointLightComponent->SetupAttachment(RootComponent);
		PointLightComponent->SetLightColor(FLinearColor::Red);
		PointLightComponent->SetIndirectLightingIntensity(10.f);
		PointLightComponent->Activate();
	}

	// Load material for projectile from unreal files
	static ConstructorHelpers::FObjectFinder<UMaterial>Material(TEXT("Material'/Game/Fruitcake_Game/Materials/ProjectileM.ProjectileM'"));
	if (Material.Succeeded())
	{
		ProjectileMaterialInstance = UMaterialInstanceDynamic::Create(Material.Object, ProjectileMeshComponent);
	}
	ProjectileMeshComponent->SetMaterial(0, ProjectileMaterialInstance);
	ProjectileMeshComponent->BodyInstance.SetCollisionProfileName(TEXT("EnemyProjectile"));

	ProjectileMeshComponent->SetRelativeScale3D(FVector(0.25f, 0.25f, 0.25f));
	ProjectileMeshComponent->SetupAttachment(RootComponent);

	// Event called when component hits something.
	CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("EnemyProjectile"));
	CollisionComponent->OnComponentHit.AddDynamic(this, &AProjectiles::OnHit);

}

// Called when the game starts or when spawned
void AProjectiles::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

}

// Called every frame
void AProjectiles::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectiles::FireInDirection(const FVector& ShootDirection, bool isHoming)
{
	// Sets velocity vector to be the direction multipled by the initial speed of the projectile
	ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
	ProjectileMovementComponent->HomingTargetComponent = PlayerCharacter->GetRootComponent();

	//// Once projecitle is fired, check to see if projectile was set to homing
	if (isHoming)
	{
		// if set to homing, wait half a second before targetting enemy
		GetWorldTimerManager().SetTimer(ProjectileTimerHandle, this, &AProjectiles::HomingOnTarget, .5f, false);
	}
}



void AProjectiles::HomingOnTarget()
{
	// sets target to enemy collision component
		// sets target to enemy collision component
	ProjectileMovementComponent->bIsHomingProjectile = false;
}

void AProjectiles::GetTarget()
{
}

void AProjectiles::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor != this)
	{
		PlayerCharacter->ReducePlayerHealth();
		Destroy();
	}
}


