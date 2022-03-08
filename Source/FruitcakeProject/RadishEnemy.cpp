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
		RadishEnemyMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMeshComponent"));
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
		WeakPointMeshComponent->SetWorldScale3D(FVector(0.5f, 0.5f, 0.5f));
		WeakPointMeshComponent->SetWorldLocation(FVector(-25, 0, -50));
		// set how long projectile will last in seconds, after this amount of time, projectile is destroyed
		//InitialLifeSpan = 3.f;
	}
	WeakPointMeshComponent->SetupAttachment(RootComponent);
	WeakPointMeshComponent->SetCollisionProfileName(TEXT("Enemy"));

	//sight sphere component set up
	if (!SightSphere)
	{
		SightSphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
		SightSphere->SetSphereRadius(750.f);

		SightSphere->SetupAttachment(RootComponent);
	}

	CollisionComponent->SetCollisionProfileName(TEXT("Enemy"));
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ARadishEnemy::OnOverlapBegin);
	CollisionComponent->OnComponentEndOverlap.AddDynamic(this, &ARadishEnemy::OnOverlapEnd);

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

	//GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &ARadishEnemy::FireAtPlayer, 3.f, true, 0.5f);
	//GetWorldTimerManager().SetTimer(AoeAttackTimerHandle, this, &ARadishEnemy::FireAoeAtPlayer, 5.f, true, 2.f);
}

// Called every frame
void ARadishEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//	FireAtPlayer();

	//if (bHostile)
	//{
	//	GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, TEXT("follow"));
	//	FVector Direction = PlayerCharacter->GetActorLocation() - GetActorLocation();
	//	Direction.Normalize();

	//	Direction = Direction * DeltaTime * MovementSpeed;

	//	//SetActorLocation(GetActorLocation() + Direction);
	//}
	//else
	//{
	//	GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, TEXT("patrol"));
	//}
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

void ARadishEnemy::SetStunned()
{
	bStunned = false;
	RadishEnemyMeshComponent->SetMaterial(0, default_material);
}

void ARadishEnemy::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherComp->ComponentHasTag(FName("PlayerAttack")))
	{
		bStunned = true;
		GetWorldTimerManager().SetTimer(StunTimerHandle, this, &ARadishEnemy::SetStunned, 0.5f, false, 5.f);

		RadishEnemyMeshComponent->SetMaterial(0, red_material);
	}
	if (OtherComp->ComponentHasTag(FName("PlayerBody")) && bStunned == false)
	{
		PlayerCharacter->ReducePlayerHealth();
	}
}

void ARadishEnemy::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
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