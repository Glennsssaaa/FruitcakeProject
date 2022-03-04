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
		CollisionComponent->SetBoxExtent(FVector(60.f, 60.f, 60.f));
		// Set the root component to be newly created component.
		CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Enemy"));

		RootComponent = CollisionComponent;
	}

	// mesh component set up
	if (!RadishEnemyMeshComponent)
	{
		// sets mesh of projectile to basic sphere mesh, loaded from unreal engine files
		RadishEnemyMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComponent"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Cylinder.Shape_Cylinder'"));
		if (Mesh.Succeeded())
		{
			RadishEnemyMeshComponent->SetStaticMesh(Mesh.Object);
		}
		RadishEnemyMeshComponent->SetWorldLocation(FVector(0, 0, -50));
		// set how long projectile will last in seconds, after this amount of time, projectile is destroyed
		//InitialLifeSpan = 3.f;
	}
	RadishEnemyMeshComponent->SetupAttachment(RootComponent);
	RadishEnemyMeshComponent->SetCollisionProfileName(TEXT("Enemy"));

	CollisionComponent->SetCollisionProfileName(TEXT("Enemy"));
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ARadishEnemy::OnOverlapBegin);
	CollisionComponent->OnComponentEndOverlap.AddDynamic(this, &ARadishEnemy::OnOverlapEnd);
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

	GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &ARadishEnemy::FireAtPlayer, 3.f, true, 0.5f);
	GetWorldTimerManager().SetTimer(AoeAttackTimerHandle, this, &ARadishEnemy::FireAoeAtPlayer, 5.f, true, 2.f);
}

// Called every frame
void ARadishEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//	FireAtPlayer();
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
				Projectile->FireInDirection(LaunchDirection, true);
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

void ARadishEnemy::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherComp->ComponentHasTag(FName("PlayerAttack")))
	{
		Destroy();
	}
}

void ARadishEnemy::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}
