#include "Projectiles.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "PlayerCharacter.h"
#include "Particles/ParticleSystem.h"
#include "NiagaraComponent.h"
#include "FlowerEnemy.h"
#include "Particles/ParticleSystemComponent.h"


// Sets default values
AProjectiles::AProjectiles()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Load material for projectile from unreal files
	static ConstructorHelpers::FObjectFinder<UMaterial>Material1(TEXT("Material'/Game/Fruitcake_Game/Materials/M_GuideLaser.M_GuideLaser'"));
	if (Material1.Succeeded())
	{
		ProjectileMaterialInstancePlayer = UMaterialInstanceDynamic::Create(Material1.Object, ProjectileMeshComponent);
	}

	static ConstructorHelpers::FObjectFinder<UMaterial>Material2(TEXT("Material'/Game/Fruitcake_Game/Materials/M_EnemyProjectile.M_EnemyProjectile'"));
	if (Material2.Succeeded())
	{
		ProjectileMaterialInstanceEnemy = UMaterialInstanceDynamic::Create(Material2.Object, ProjectileMeshComponent);
	}
	
	// collision component set up
	if (!CollisionComponent)
	{
		// Set Collision box to be sphere.
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
			ProjectileMeshComponent->SetWorldScale3D(FVector(0.25f, 0.25f, 0.25f));
		}
		// set how long projectile will last in seconds, after this amount of time, projectile is destroyed
		InitialLifeSpan = 3.f;
	}

	ProjectileMeshComponent->BodyInstance.SetCollisionProfileName(TEXT("EnemyProjectile"));

	//ProjectileMeshComponent->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));
	ProjectileMeshComponent->SetupAttachment(RootComponent);
	
	// projectile movement component set up
	if (!ProjectileMovementComponent)
	{
		// Use this component to drive projectile movement.
		ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));

		// assign the collision component to movement component
		ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);

		// set up projectile initial and maximum speeds
		ProjectileMovementComponent->InitialSpeed = 1500.0f;
		ProjectileMovementComponent->MaxSpeed = 1500.0f;

		ProjectileMovementComponent->bRotationFollowsVelocity = true;
		ProjectileMovementComponent->ProjectileGravityScale = 0.0f;

		// set if projectile is homing or not
		ProjectileMovementComponent->bIsHomingProjectile = true;
		// set how quickly projectile will home in
		ProjectileMovementComponent->HomingAccelerationMagnitude = 3000.f;
	}


	if (!ProjectileParticleEffectN)
	{
		ProjectileParticleEffectN = CreateDefaultSubobject<UNiagaraSystem>(TEXT("ParticleEffectN"));
		static ConstructorHelpers::FObjectFinder<UNiagaraSystem>ParticleN(TEXT("NiagaraSystem'/Game/Fruitcake_Game/VFX/ProjectileEffect_Converted.ProjectileEffect_Converted'"));
		if (ParticleN.Succeeded())
		{
			ProjectileParticleEffectN = ParticleN.Object;
		}
	}

	if (!PointLightComponent)
	{
		PointLightComponent = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));
		PointLightComponent->SetupAttachment(RootComponent);
	}

}

// Called when the game starts or when spawned
void AProjectiles::BeginPlay()
{
	InitialLifeSpan = 1.5f;
	Super::BeginPlay();

	PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}

// Called every frame
void AProjectiles::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (ParticleComponent)
	{
		ParticleComponent->SetWorldLocation(GetActorLocation());
	}
}

void AProjectiles::FireInDirection(const FVector& ShootDirection, bool isHoming, bool isPlayer)
{
	ParticleComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ProjectileParticleEffectN, GetActorLocation());
	// sets if projectile is coming from the player or not, used in collision checks
	isPlayerProjectile = isPlayer;
	if (!isPlayer)
	{
		ProjectileMeshComponent->BodyInstance.SetCollisionProfileName(TEXT("EnemyProjectile"));
		CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("EnemyProjectile"));
		ProjectileMovementComponent->bIsHomingProjectile = true;
		ProjectileMovementComponent->HomingTargetComponent = PlayerCharacter->GetRootComponent();
		//Set VFX Colour to magenta
		ProjectileMeshComponent->SetMaterial(0, ProjectileMaterialInstanceEnemy);
		PointLightComponent->SetLightColor(FLinearColor(1.f, 0.f, 1.f, 1.f));
		ParticleComponent->SetNiagaraVariableVec3(FString("ColorParamater"), FVector(1, 0, 1));
		// if set to homing, wait half a second before targeting enemy
		GetWorldTimerManager().SetTimer(ProjectileTimerHandle, this, &AProjectiles::HomingOnTarget, .5f, false);
	}
	else
	{
		ProjectileMovementComponent->InitialSpeed = 2500.0f;
		ProjectileMovementComponent->MaxSpeed = 2500.0f;
		CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("PlayerAttack"));
		ProjectileMeshComponent->BodyInstance.SetCollisionProfileName(TEXT("PlayerAttack"));
		//Set VFX Colour to cyan
		ProjectileMeshComponent->SetMaterial(0, ProjectileMaterialInstancePlayer);
		PointLightComponent->SetLightColor(FLinearColor(0.f, 1.f, 1.f, 1.f));
		ParticleComponent->SetNiagaraVariableVec3(FString("ColorParamater"), FVector(0, 1, 1));
	}

	
	// Sets velocity vector to be the direction multiplied by the initial speed of the projectile

	ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;

	// Once projectile is fired, check to see if projectile was set to homing

}



void AProjectiles::HomingOnTarget() const
{
	// sets target to enemy collision component
	ProjectileMovementComponent->bIsHomingProjectile = false;
}

void AProjectiles::GetTarget()
{
}

void AProjectiles::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor == nullptr || OtherComp == nullptr || OtherActor == this)
	{
		return;
	}
	
	// Enemy Projectile Collision Responses
	if (OtherComp->ComponentHasTag(FName(TEXT("Enemy"))))
	{
		OtherActor->TakeDamage(1.f, FDamageEvent(), nullptr, this);
	}
	
	if (!isPlayerProjectile)
	{
		ParticleComponent->Deactivate();
		Destroy();
	}
	
	
	// Player Projectile Collision Responses
	if (isPlayerProjectile && !OtherActor->IsA(APlayerCharacter::StaticClass()) && OtherComp->GetCollisionProfileName() != FName("DoorButton") && OtherComp->GetCollisionProfileName() != FName("AICollision"))
	{
		ParticleComponent->Deactivate();
		Destroy();
	}
}

void AProjectiles::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if(!isPlayerProjectile && OtherComponent->GetCollisionProfileName() == (FName(TEXT("Player"))))
	{
		PlayerCharacter->TakeDamage(1.f, FDamageEvent(), nullptr, this);
	}
	ParticleComponent->Deactivate();
	Destroy();
}
