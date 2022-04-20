// Fill out your copyright notice in the Description page of Project Settings.

#include "MushroomEnemy.h"
#include "PlayerCharacter.h"
#include "Projectiles.h"
#include "AoeAttackController.h"

// Sets default values
AMushroomEnemy::AMushroomEnemy()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!CollisionComponent)
	{
		// Set Collision box to be sphere.
		CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
		// Set collision box radius.
		CollisionComponent->SetBoxExtent(FVector(120.f, 120.f, 120.f));
		// Set the root component to be newly created component.
		CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Enemy"));

		RootComponent = CollisionComponent;
	}
	// mesh component set up
	if (!MushroomEnemyMeshComponent)
	{
		// sets mesh of projectile to basic sphere mesh, loaded from unreal engine files
		MushroomEnemyMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComponent"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Cone.Shape_Cone'"));
		if (Mesh.Succeeded())
		{
			MushroomEnemyMeshComponent->SetStaticMesh(Mesh.Object);
		}
		MushroomEnemyMeshComponent->SetWorldLocation(FVector(0, 0, -50));
		MushroomEnemyMeshComponent->SetWorldScale3D(FVector(2.f, 2.f, 2.f));
		// set how long projectile will last in seconds, after this amount of time, projectile is destroyed
		InitialLifeSpan = 6.f;
	}
	MushroomEnemyMeshComponent->SetupAttachment(RootComponent);
	MushroomEnemyMeshComponent->SetCollisionProfileName(TEXT("Enemy"));

	CollisionComponent->SetCollisionProfileName(TEXT("Enemy"));
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AMushroomEnemy::OnOverlapBegin);
	CollisionComponent->OnComponentEndOverlap.AddDynamic(this, &AMushroomEnemy::OnOverlapEnd);

	//sight sphere component set up
	if (!SightSphere)
	{
		SightSphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
		SightSphere->SetSphereRadius(750.f);

		SightSphere->SetupAttachment(RootComponent);
	}

	CollisionComponent->SetCollisionProfileName(TEXT("AICollision"));
	SightSphere->OnComponentBeginOverlap.AddDynamic(this, &AMushroomEnemy::OnTriggerBegin);
	SightSphere->OnComponentEndOverlap.AddDynamic(this, &AMushroomEnemy::OnTriggerEnd);
}

// Called when the game starts or when spawned
void AMushroomEnemy::BeginPlay()
{
	Super::BeginPlay();

	//EnergySphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnergyOrbs::OnOverlapBegin);
	//EnergySphereCollision->OnComponentEndOverlap.AddDynamic(this, &AEnergyOrbs::OnOverlapEnd);

	PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	MushroomProjectiles = AProjectiles::StaticClass();
	MushroomAoeAttacks = AAoeAttackController::StaticClass();

	//GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &AMushroomEnemy::FireAtPlayer, 3.f, true, 0.5f);
	GetWorldTimerManager().SetTimer(AoeAttackTimerHandle, this, &AMushroomEnemy::FireAoeAtPlayer, 3.f, true, 2.f);
}

// Called every frame
void AMushroomEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	JumpTowardsPlayer();
	//	FireAtPlayer();
}

void AMushroomEnemy::FireAtPlayer()
{

	if (MushroomProjectiles)
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
			AProjectiles* Projectile = World->SpawnActor<AProjectiles>(MushroomProjectiles, SpawnLocation, FRotator(0.f, 0.f, 0.f), SpawnParams);

			if (Projectile)
			{
				// Set the projectile's initial trajectory.
				FVector LaunchDirection = MuzzleRotation.Vector();
				Projectile->FireInDirection(LaunchDirection, true, false);
			}

		}
	}
}

void AMushroomEnemy::FireAoeAtPlayer()
{
	if (MushroomAoeAttacks)
	{
		FVector SpawnLocation;
		FRotator CameraRotation;
		//SpawnLocation = FVector(PlayerCharacter->GetActorLocation().X, PlayerCharacter->GetActorLocation().Y, PlayerCharacter->GetActorLocation().Z - 90.f);
		SpawnLocation = FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z - 65.f);

		// set rotation of projectile to camera rotation
		FRotator MuzzleRotation = CameraRotation;

		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();

			// Spawn the projectile at the muzzle.
			AAoeAttackController* Projectile = World->SpawnActor<AAoeAttackController>(MushroomAoeAttacks, SpawnLocation, FRotator(0.f, 0.f, 0.f), SpawnParams);

			if (Projectile)
			{
				// Set the projectile's initial trajectory.
				FVector LaunchDirection = MuzzleRotation.Vector();
				Projectile->FireAtLocation(LaunchDirection, 7.f);
			}

		}
	}
}

void AMushroomEnemy::JumpTowardsPlayer() {
	FVector PlayerLocation = PlayerCharacter->GetActorLocation();
	FVector EnemyLocation = GetActorLocation();
	FVector JumpDirection = PlayerLocation - EnemyLocation;
	JumpDirection.Normalize();
	JumpDirection *= 3;
	SetActorLocation(GetActorLocation() + JumpDirection);


	
}

void AMushroomEnemy::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherComp->ComponentHasTag(FName("PlayerAttack")))
	{
		SightSphere->OnComponentBeginOverlap.Clear();
		Destroy();
	}

	if (OtherComp->GetCollisionProfileName() == TEXT("PlayerAttack"))
	{
		OtherActor->Destroy();
		Destroy();
	}
}

void AMushroomEnemy::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void AMushroomEnemy::OnTriggerBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherComp->GetCollisionProfileName() == TEXT("Player"))
	{
		bHostile = true;
	}
}

void AMushroomEnemy::OnTriggerEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherComp->GetCollisionProfileName() == TEXT("Player"))
	{
		bHostile = false;
	}
}