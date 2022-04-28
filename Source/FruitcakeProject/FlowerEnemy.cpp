// Fill out your copyright notice in the Description page of Project Settings.

#include "FlowerEnemy.h"
#include "PlayerCharacter.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Projectiles.h"
#include "AoeAttackController.h"

// Sets default values
AFlowerEnemy::AFlowerEnemy()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!CollisionComponent)
	{
		// Set Collsion box to be sphere.
		CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
		// Set collision box radius.
		CollisionComponent->SetBoxExtent(FVector(90.f, 90.f, 90.f));
		// Set the root component to be newly created component.
		CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Enemy"));

		RootComponent = CollisionComponent;
	}

	// mesh component set up
	if (!FlowerEnemyMeshComponent)
	{
		// sets mesh of projectile to basic sphere mesh, loaded from unreal engine files
		FlowerEnemyMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComponent"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube'"));
		if (Mesh.Succeeded())
		{
			FlowerEnemyMeshComponent->SetStaticMesh(Mesh.Object);
		}
		FlowerEnemyMeshComponent->SetWorldLocation(FVector(0, 0, 0));
		FlowerEnemyMeshComponent->SetWorldScale3D(FVector(1.3f, 1.3f, 1.3f));
		// set how long projectile will last in seconds, after this amount of time, projectile is destroyed
		InitialLifeSpan = 3.f;
	}
	FlowerEnemyMeshComponent->SetupAttachment(RootComponent);
	FlowerEnemyMeshComponent->SetCollisionProfileName(TEXT("Enemy"));

	CollisionComponent->SetCollisionProfileName(TEXT("Enemy"));
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AFlowerEnemy::OnOverlapBegin);
	CollisionComponent->OnComponentEndOverlap.AddDynamic(this, &AFlowerEnemy::OnOverlapEnd);


}

// Called when the game starts or when spawned
void AFlowerEnemy::BeginPlay()
{
	Super::BeginPlay();

	//EnergySphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnergyOrbs::OnOverlapBegin);
	//EnergySphereCollision->OnComponentEndOverlap.AddDynamic(this, &AEnergyOrbs::OnOverlapEnd);

	PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	FlowerProjectiles = AProjectiles::StaticClass();
	FlowerAoeAttacks = AAoeAttackController::StaticClass();

	GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &AFlowerEnemy::FireAtPlayer, 3.f, true, 0.5f);
}

// Called every frame
void AFlowerEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFlowerEnemy::FireAtPlayer()
{

	if (FlowerProjectiles)
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
		FRotator MuzzleRotation = GetActorRotation();

		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams = FActorSpawnParameters();
			SpawnParams.Owner = this;
			//SpawnParams.Name = FName(TEXT("AFlowerEnemy"));
			SpawnParams.Instigator = GetInstigator();

			// Spawn the projectile at the muzzle.
			AProjectiles* Projectile = World->SpawnActor<AProjectiles>(FlowerProjectiles, SpawnLocation, FRotator(0.f, 0.f, 0.f), SpawnParams);

			if (Projectile)
			{
				// Set the projectile's initial trajectory.
				FVector LaunchDirection = MuzzleRotation.Vector();
				Projectile->FireInDirection(LaunchDirection, true, false);
			}
		}
	}
}

void AFlowerEnemy::FireAoeAtPlayer()
{
	if (FlowerAoeAttacks)
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
			AAoeAttackController* Projectile = World->SpawnActor<AAoeAttackController>(FlowerAoeAttacks, SpawnLocation, FRotator(0.f, 0.f, 0.f), SpawnParams);

			if (Projectile)
			{
				// Set the projectile's initial trajectory.
				FVector LaunchDirection = MuzzleRotation.Vector();
				Projectile->FireAtLocation(LaunchDirection, 3.f);
			}

		}
	}
}

void AFlowerEnemy::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherComp->ComponentHasTag(FName("PlayerAttack")))
	{
		Destroy();
	}

	if (OtherComp->GetCollisionProfileName() == TEXT("PlayerAttack"))
	{
		OtherActor->Destroy();
		Destroy();
	}
}

void AFlowerEnemy::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}