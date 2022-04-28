#include "Projectiles.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "PlayerCharacter.h"
#include "Engine/Engine.h"
#include "FlowerEnemy.h"


// Sets default values
AProjectiles::AProjectiles()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Load material for projectile from unreal files

	//Causing error message on startup
	/*static ConstructorHelpers::FObjectFinder<UMaterial>Material(TEXT("Material'/Game/Fruitcake_Game/Materials/Material_GuideLaser.Material_GuideLaser'"));
	if (Material.Succeeded())
	{
		ProjectileMaterialInstance = UMaterialInstanceDynamic::Create(Material.Object, ProjectileMeshComponent);
	}*/
	
	// collision component set up
	if (!CollisionComponent)
	{
		// Set Collsion box to be sphere.
		CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
		// Set collision box radius.
		CollisionComponent->InitSphereRadius(30.0f);
		// Set the root component to be newly created component.
		RootComponent = CollisionComponent;
	}
	// Event called when component hits something.
	CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("EnemyProjectile"));
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AProjectiles::OnOverlap);
	CollisionComponent->OnComponentHit.AddDynamic(this, &AProjectiles::OnHit);


	// mesh component set up
	if (!ProjectileMeshComponent)
	{
		// sets mesh of projectile to basic sphere mesh, loaded from unreal engine files
		ProjectileMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComponent"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh(TEXT("StaticMesh'/Game/StarterContent/Props/MaterialSphere.MaterialSphere'"));
		if (Mesh.Succeeded())
		{
			ProjectileMeshComponent->SetStaticMesh(Mesh.Object);
			ProjectileMeshComponent->SetWorldScale3D(FVector(2.f, 2.f, 2.f));
		}
		// set how long projectile will last in seconds, after this amount of time, projectile is destroyed
		InitialLifeSpan = 3.f;
	}


	ProjectileMeshComponent->SetMaterial(0, ProjectileMaterialInstance);
	ProjectileMeshComponent->BodyInstance.SetCollisionProfileName(TEXT("EnemyProjectile"));

	ProjectileMeshComponent->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));
	ProjectileMeshComponent->SetupAttachment(RootComponent);
	
	// projectile movement component set up
	if (!ProjectileMovementComponent)
	{
		// Use this component to drive projectile movement.
		ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));

		// assign the collision component to movement component
		ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);

		// set up projecitle initial and maximum speeds
		ProjectileMovementComponent->InitialSpeed = 1500.0f;
		ProjectileMovementComponent->MaxSpeed = 1500.0f;

		ProjectileMovementComponent->bRotationFollowsVelocity = true;
		ProjectileMovementComponent->ProjectileGravityScale = 0.0f;

		// set if projectile is homing or not
		ProjectileMovementComponent->bIsHomingProjectile = true;
		// set how quickly projectile will home in
		ProjectileMovementComponent->HomingAccelerationMagnitude = 3000.f;
	}




	if (!ProjectileParticleEffect)
	{
		ProjectileParticleEffect = CreateDefaultSubobject<UParticleSystem>(TEXT("ParticleEffect"));
		static ConstructorHelpers::FObjectFinder<UParticleSystem>Particle(TEXT("ParticleSystem'/Game/Fruitcake_Game/VFX/ProjectileEffect.ProjectileEffect'"));
		if (Particle.Succeeded())
		{
			ProjectileParticleEffect = Particle.Object;
		}
	}

	if (!PointLightComponent)
	{
		PointLightComponent = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));
		PointLightComponent->SetupAttachment(RootComponent);
		PointLightComponent->SetLightColor(FLinearColor(0.f, 0.5f, 1.f, 1.f));
	}

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
	temptime += DeltaTime;

}

void AProjectiles::FireInDirection(const FVector& ShootDirection, bool isHoming, bool isPlayer)
{
	// sets if projectile is coming from the player or not, used in collision checks
	isPlayerProjectile = isPlayer;
	if (!isPlayer)
	{
		ProjectileMeshComponent->BodyInstance.SetCollisionProfileName(TEXT("EnemyProjectile"));
		CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("EnemyProjectile"));
		ProjectileMovementComponent->bIsHomingProjectile = true;
		ProjectileMovementComponent->HomingTargetComponent = PlayerCharacter->GetRootComponent();
		// if set to homing, wait half a second before targetting enemy
		GetWorldTimerManager().SetTimer(ProjectileTimerHandle, this, &AProjectiles::HomingOnTarget, .5f, false);
	}
	else
	{
		CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("PlayerAttack"));
		ProjectileMeshComponent->BodyInstance.SetCollisionProfileName(TEXT("PlayerAttack"));
	}

	
	// Sets velocity vector to be the direction multipled by the initial speed of the projectile

	ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;

	// Once projecitle is fired, check to see if projectile was set to homing



	UGameplayStatics::SpawnEmitterAttached(ProjectileParticleEffect, RootComponent);

}



void AProjectiles::HomingOnTarget()
{
	// sets target to enemy collision component
	ProjectileMovementComponent->bIsHomingProjectile = false;
}

void AProjectiles::GetTarget()
{
}

void AProjectiles::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != this)
	{
		// Enemy Projectile Collision Responses
		if (!isPlayerProjectile)
		{
			if (OtherComp->ComponentHasTag(FName(TEXT("Player"))))
			{
				PlayerCharacter->ReducePlayerHealth();
			}
			Destroy();
		}

		if (OtherComp->ComponentHasTag(FName(TEXT("Enemy"))))
		{
			OtherActor->TakeDamage(1.f, FDamageEvent(), nullptr, this);
		}


	}
	// Player Projectile Collision Responses
	if (isPlayerProjectile && !OtherActor->IsA(APlayerCharacter::StaticClass()) && OtherComp->GetCollisionProfileName() != FName("DoorButton") && OtherComp->GetCollisionProfileName() != FName("AICollision"))
	{
		Destroy();
	}
}

void AProjectiles::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if(!isPlayerProjectile && OtherComponent->GetCollisionProfileName() == (FName(TEXT("Player"))))
	{
		PlayerCharacter->TakeDamage(1.f, FDamageEvent(), nullptr, this);
	}
	Destroy();
}
