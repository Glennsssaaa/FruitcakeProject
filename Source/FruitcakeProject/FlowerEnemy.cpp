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
		// Set Collision box to be sphere.
		CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
		// Set collision box radius.
		CollisionComponent->SetBoxExtent(FVector(90.f, 90.f, 90.f));
		// Set the root component to be newly created component.
		CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Enemy"));

		RootComponent = CollisionComponent;
	}
	
	CollisionComponent->SetCollisionProfileName(TEXT("Enemy"));
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AFlowerEnemy::OnOverlapBegin);
	CollisionComponent->OnComponentEndOverlap.AddDynamic(this, &AFlowerEnemy::OnOverlapEnd);
}

// Called when the game starts or when spawned
void AFlowerEnemy::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	FlowerProjectiles = AProjectiles::StaticClass();

	GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &AFlowerEnemy::CallFireAtPlayer, 4.f, true, 0.5f);
}

// Called every frame
void AFlowerEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Rotate the enemy to face the player.
	FVector Direction = PlayerCharacter->GetActorLocation() - GetActorLocation();
	FRotator NewRotation = FRotationMatrix::MakeFromX(Direction).Rotator();
	NewRotation.Pitch = 0.f;
	NewRotation.Roll = 0.f;
	SetActorRotation(NewRotation);

	
	
}

void AFlowerEnemy::CallFireAtPlayer()
{
	// Raycast to see if player is in sight
	FHitResult HitResult;
	FVector Start = GetActorLocation();
	FVector End = Start + (GetActorForwardVector() * 3000.f);
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams);

	if(!bHit)
	{
		return;
	}

	if(HitResult.GetActor()->IsA(APlayerCharacter::StaticClass()) == false)
	{
		return;
	}
	
	bIsShooting = true;
}


void AFlowerEnemy::FireAtPlayer()
{
	if (FlowerProjectiles)
	{
		FRotator CameraRotation;
		//	GetActorEyesViewPoint(CameraLocation, CameraRotation);


		FVector SpawnLocation = FVector(ProjectileSpawnPoint.X, ProjectileSpawnPoint.Y, ProjectileSpawnPoint.Z);
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