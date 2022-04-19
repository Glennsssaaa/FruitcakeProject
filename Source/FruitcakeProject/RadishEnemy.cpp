#include "RadishEnemy.h"
#include "PlayerCharacter.h"
#include "Projectiles.h"
#include "AoeAttackController.h"


// Sets default values
ARadishEnemy::ARadishEnemy()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!CollisionComponent)
	{
		// Set Collsion box to be sphere.
		CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
		// Set collision box radius.
		CollisionComponent->SetBoxExtent(FVector(125.f, 125.f, 100.f));
		// Set the root component to be newly created component.
		CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Enemy"));

	//	RootComponent = CollisionComponent;
	}

	if (!WeakPointMeshComponent)
	{
		// sets mesh of projectile to basic sphere mesh, loaded from unreal engine files
		WeakPointMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeakPointMeshComponent"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere'"));
		if (Mesh.Succeeded())
		{
			WeakPointMeshComponent->SetStaticMesh(Mesh.Object);
			static ConstructorHelpers::FObjectFinder<UMaterial>Mat(TEXT("Material'/Game/Fruitcake_Game/Materials/ProjectileM.ProjectileM'"));
			WeakPointMeshComponent->SetMaterial(0, Mat.Object);
		}
		WeakPointMeshComponent->SetWorldLocation(FVector(-50, 0, -50));
		// set how long projectile will last in seconds, after this amount of time, projectile is destroyed
		//InitialLifeSpan = 3.f;
	}
	WeakPointMeshComponent->SetupAttachment(RootComponent);

	//sight sphere component set up
	if (!SightSphere)
	{
		SightSphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
		SightSphere->SetSphereRadius(1000.f);

		SightSphere->SetupAttachment(RootComponent);
	}

	CollisionComponent->SetCollisionProfileName(TEXT("Enemy"));
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ARadishEnemy::OnOverlapBegin);
	CollisionComponent->OnComponentEndOverlap.AddDynamic(this, &ARadishEnemy::OnOverlapEnd);

	CollisionComponent->OnComponentHit.AddDynamic(this, &ARadishEnemy::OnHit);
	WeakPointMeshComponent->SetCollisionProfileName(TEXT("AICollision"));
	WeakPointMeshComponent->OnComponentBeginOverlap.AddDynamic(this, &ARadishEnemy::OnWeakPointOverlapBegin);

	SightSphere->SetCollisionProfileName(TEXT("AICollision"));
	SightSphere->OnComponentBeginOverlap.AddDynamic(this, &ARadishEnemy::OnTriggerBegin);
	SightSphere->OnComponentEndOverlap.AddDynamic(this, &ARadishEnemy::OnTriggerEnd);

	bHostile = false;
	bStunned = false;
	MovementSpeed = 500.f;

	static ConstructorHelpers::FObjectFinder<UMaterial>Material(TEXT("Material'/Game/StarterContent/Materials/M_Basic_Wall.M_Basic_Wall'"));
	default_material = Material.Object;

	static ConstructorHelpers::FObjectFinder<UMaterial>Material1(TEXT("Material'/Game/Fruitcake_Game/Materials/Red.Red'"));
	red_material = Material1.Object;

	dead = false;
}

// Called when the game starts or when spawned
void ARadishEnemy::BeginPlay()
{
	Super::BeginPlay();

	//EnergySphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnergyOrbs::OnOverlapBegin);
	//EnergySphereCollision->OnComponentEndOverlap.AddDynamic(this, &AEnergyOrbs::OnOverlapEnd);

	PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	RadishProjectiles = AProjectiles::StaticClass();
	RadishAoeAttacks = AAoeAttackController::StaticClass();

	health_pool = 1;


}

// Called every frame
void ARadishEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (dead)
	{
		deathTimer -= DeltaTime;
	}
}

void ARadishEnemy::FireAtPlayer()
{

	if (RadishProjectiles)
	{
		FVector SpawnLocation;
		FRotator CameraRotation;
		//	GetActorEyesViewPoint(CameraLocation, CameraRotation);


		SpawnLocation = FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z);
		// Set MuzzleOffset to spawn projectiles slightly in front of the camera.
		MuzzleOffset.Set(100.0f, 40.0f, 0.0f);

		// Transform MuzzleOffset from camera space to world space.
	//	FVector MuzzleLocation = CameraLocation + FTransform(CameraRotation).TransformVector(MuzzleOffset);

		// set rotation of projectile to camera rotation
		FRotator MuzzleRotation = CameraRotation;

		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();

			// Spawn the projectile at the muzzle.
			AProjectiles* Projectile = World->SpawnActor<AProjectiles>(RadishProjectiles, SpawnLocation, FRotator(0.f, 0.f, 0.f), SpawnParams);

			if (Projectile)
			{
				// Set the projectile's initial trajectory.
				FVector LaunchDirection = MuzzleRotation.Vector();
				Projectile->FireInDirection(LaunchDirection, true, false);
			}

		}
	}
}

void ARadishEnemy::FireAoeAtPlayer()
{
	if (RadishAoeAttacks)
	{
		FVector SpawnLocation;
		FRotator CameraRotation;
		SpawnLocation = FVector(PlayerCharacter->GetActorLocation().X, PlayerCharacter->GetActorLocation().Y, PlayerCharacter->GetActorLocation().Z - 90.f);

		// set rotation of projectile to camera rotation
		FRotator MuzzleRotation = CameraRotation;

		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();

			// Spawn the projectile at the muzzle.
			AAoeAttackController* Projectile = World->SpawnActor<AAoeAttackController>(RadishAoeAttacks, SpawnLocation, FRotator(0.f, 0.f, 0.f), SpawnParams);

			if (Projectile)
			{
				// Set the projectile's initial trajectory.
				FVector LaunchDirection = MuzzleRotation.Vector();
				Projectile->FireAtLocation(LaunchDirection, 3.f);
			}

		}
	}
}

void ARadishEnemy::SetStunned()
{
	bStunned = false;
}

void ARadishEnemy::Kill()
{
	Destroy();
}

void ARadishEnemy::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherComp->ComponentHasTag(FName("PlayerAttack")))
	{
		health_pool--;
		if (health_pool <= 0) 
		{
			// to do - unbind delegates 

			dead = true;
			deathTimer = 6.5f;
		}
	}
}

void ARadishEnemy::OnWeakPointOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherComp->ComponentHasTag(FName("PlayerAttack")) && bStunned == true)
	{
		//Destroy();
	}
}

void ARadishEnemy::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void ARadishEnemy::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherComponent->ComponentHasTag(FName("Player")) && bStunned == false)
	{
	//	PlayerCharacter->ReducePlayerHealth();
	//	bStunned = true;
		//GetWorldTimerManager().SetTimer(StunTimerHandle, this, &ARadishEnemy::SetStunned, 0.5f, false, 5.f);

	}
}

void ARadishEnemy::OnTriggerBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherComp->GetCollisionProfileName() == TEXT("Player"))
	{
		bHostile = true;
	}
}

void ARadishEnemy::OnTriggerEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherComp->GetCollisionProfileName() == TEXT("Player"))
	{
		bHostile = false;
	}
}

void ARadishEnemy::ReducePlayerHealth()
{
	PlayerCharacter->ReducePlayerHealth();
}
