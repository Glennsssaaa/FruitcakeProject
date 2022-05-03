#include "RadishEnemy.h"
#include "PlayerCharacter.h"
#include "Projectiles.h"
#include "AoeAttackController.h"

// Sets default values
ARadishEnemy::ARadishEnemy()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Main Mesh Collision Component Set up

	if (!CollisionComponent)
	{
		// Set Collsion box to be sphere.
		CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
		// Set collision box radius.
		CollisionComponent->SetBoxExtent(FVector(125.f, 125.f, 100.f));
		// Set the root component to be newly created component.
		CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Enemy"));

		RootComponent = CollisionComponent;
	}

	CollisionComponent->SetCollisionProfileName(TEXT("Enemy"));
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ARadishEnemy::OnOverlapBegin);
	CollisionComponent->OnComponentEndOverlap.AddDynamic(this, &ARadishEnemy::OnOverlapEnd);
	CollisionComponent->OnComponentHit.AddDynamic(this, &ARadishEnemy::OnHit);
	
	// Sight Sphere Component Set Up

	if (!SightSphere)
	{
		SightSphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
		SightSphere->InitSphereRadius(1000.f);
		SightSphere->SetupAttachment(RootComponent);
	}
	SightSphere->SetCollisionProfileName(TEXT("AICollision"));
	SightSphere->OnComponentBeginOverlap.AddDynamic(this, &ARadishEnemy::OnTriggerBegin);
	SightSphere->OnComponentEndOverlap.AddDynamic(this, &ARadishEnemy::OnTriggerEnd);

	// Attack Range Component Set Up

	if (!AttackRange)
	{
		AttackRange = CreateDefaultSubobject<USphereComponent>(TEXT("AttackRangeComponent"));
		AttackRange->InitSphereRadius(400.f);
		AttackRange->SetupAttachment(RootComponent);
	}
	AttackRange->SetCollisionProfileName(TEXT("EnemyAOE"));
	AttackRange->OnComponentBeginOverlap.AddDynamic(this, &ARadishEnemy::OnAttackRangeOverlapBegin);
	AttackRange->OnComponentEndOverlap.AddDynamic(this, &ARadishEnemy::OnAttackRangeOverlapEnd);

	// Weak Point Component Set Up

	//if (!WeakPointMeshComponent)
	//{
	//	// sets mesh of projectile to basic sphere mesh, loaded from unreal engine files
	//	WeakPointMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeakPointMeshComponent"));
	//	static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere'"));
	//	if (Mesh.Succeeded())
	//	{
	//		WeakPointMeshComponent->SetStaticMesh(Mesh.Object);
	//		static ConstructorHelpers::FObjectFinder<UMaterial>Mat(TEXT("Material'/Game/Fruitcake_Game/Materials/ProjectileM.ProjectileM'"));
	//		WeakPointMeshComponent->SetMaterial(0, Mat.Object);
	//	}
	//	WeakPointMeshComponent->SetWorldLocation(FVector(-50, 0, -50));
	//}
	//WeakPointMeshComponent->SetupAttachment(RootComponent);
	//WeakPointMeshComponent->SetCollisionProfileName(TEXT("AICollision"));
	//WeakPointMeshComponent->OnComponentBeginOverlap.AddDynamic(this, &ARadishEnemy::OnWeakPointOverlapBegin);	




	static ConstructorHelpers::FObjectFinder<UMaterial>Material(TEXT("Material'/Game/StarterContent/Materials/M_Basic_Wall.M_Basic_Wall'"));
	default_material = Material.Object;

	static ConstructorHelpers::FObjectFinder<UMaterial>Material1(TEXT("Material'/Game/Fruitcake_Game/Materials/Red.Red'"));
	red_material = Material1.Object;
}

// Called when the game starts or when spawned
void ARadishEnemy::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));


	// Initialise hostile, stunned and attack bools
	bHostile = false;
	bStunned = false;
	bAttack = false;
	b_attack_delay_active = false;
	MovementSpeed = 20.f;
	AttackRange->SetSphereRadius(300.f);
	health_pool = 5;

	f_attack_delay_time = 0.6f;

}

// Called every frame
void ARadishEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(!dead)
	{
		if (bHostile) {
			// If enemy is hostile, move towards player
			FVector Direction = PlayerCharacter->GetActorLocation() - GetActorLocation();

			// if enemy too close to player, stop moving

			if (Direction.Size() > 100)
			{
				Direction.Normalize();
				AddMovementInput(Direction, MovementSpeed * DeltaTime);
			}


		}
		else if (bStunned) {
			// If enemy is stunned, do nothing
			FVector Direction = PlayerCharacter->GetActorLocation() + GetActorLocation();
			Direction.Normalize();
			AddMovementInput(Direction, MovementSpeed * DeltaTime);
		}
		else {
			// If enemy is not hostile, do nothing
		}
		RotateTowardsPlayer();
	}
	
}

void ARadishEnemy::RotateTowardsPlayer() {
	FVector Direction = PlayerCharacter->GetActorLocation() - GetActorLocation();
	Direction.Normalize();
	FRotator NewLookAt = FRotationMatrix::MakeFromX(Direction).Rotator();
	SetActorRotation(NewLookAt);
}
	


void ARadishEnemy::SetStunned()
{
	bStunned = false;
}

bool ARadishEnemy::CheckForOverlapMethod()
{
	return false;
}

void ARadishEnemy::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherComp->ComponentHasTag(FName("PlayerAttack")) && PlayerCharacter->GetCanDamage() == true)
	{
		PlayerCharacter->SetCanDamage();
		TakeDamage(1, FDamageEvent(), nullptr, this);
		bStunned = true;
		GetWorldTimerManager().SetTimer(StunTimerHandle, this, &ARadishEnemy::SetStunned, 0.3f, false, 0.3f);
		if (health_pool <= 0) 
		{
			// to do - unbind delegates 

			Destroy();
		}
	}
}



void ARadishEnemy::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void ARadishEnemy::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherComponent->ComponentHasTag(FName("Player")) && bStunned == false)
	{

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
	//	bHostile = false;
	}
}

void ARadishEnemy::OnAttackRangeOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(APlayerCharacter::StaticClass()) && !b_attack_delay_active)
	{
		GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &ARadishEnemy::CheckIfStillOverlapping, 0.1f, false, f_attack_delay_time);
	}
}

void ARadishEnemy::OnAttackRangeOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void ARadishEnemy::CheckIfStillOverlapping()
{
	// Checks if enemy attack range component is still overlapping with player, if so, attack again, with a delay of half a second
	if (AttackRange->IsOverlappingActor(PlayerCharacter) && !b_attack_delay_active) 
	{
		bAttack = true;
		b_attack_delay_active = true;
		GetWorldTimerManager().SetTimer(AttackDelayTimerHandle, this, &ARadishEnemy::SetAttackDelayBool, 0.1f, false, 0.5f);
	}
	else 
	{
		bAttack = false;
	}
}

void ARadishEnemy::SetAttackDelayBool() 
{
	b_attack_delay_active = false;
}

void ARadishEnemy::ReducePlayerHealth()
{
	PlayerCharacter->TakeDamage(1.f, FDamageEvent(), nullptr, this);
	GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &ARadishEnemy::CheckIfStillOverlapping, 0.1f, false, f_attack_delay_time);
}