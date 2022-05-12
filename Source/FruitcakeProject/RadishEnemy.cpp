#include "RadishEnemy.h"
#include "PlayerCharacter.h"
#include "Projectiles.h"
#include "Kismet/GameplayStatics.h"
#include "AoeAttackController.h"

// Sets default values
ARadishEnemy::ARadishEnemy()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Main Mesh Collision Component Set up

	if (!CollisionComponent)
	{
		// Set Collision box to be sphere.
		CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
		// Set collision box radius.
		CollisionComponent->SetBoxExtent(FVector(125.f, 125.f, 100.f));
		// Set the root component to be newly created component.
		CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Enemy"));

		RootComponent = CollisionComponent;
	}

	CollisionComponent->SetCollisionProfileName(TEXT("Enemy"));
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ARadishEnemy::OnOverlapBegin);

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
}

// Called when the game starts or when spawned
void ARadishEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	// Initialise hostile, stunned and attack bool
	bIsDead = false;
	bHostile = false;
	bStunned = false;
	bAttack = false;
	bAttackDelayActive = false;
	MovementSpeed = 20.f;
	AttackRange->SetSphereRadius(300.f);
	// Cast to player
	Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	bJustDamaged = false;
	AttackDelayTime = 0.6f;
}

// Called every frame
void ARadishEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if(bDead)
	{
		return;
	}
	
	if(bJustDamaged)
	{
		// Go backwards
		AddMovementInput(GetActorForwardVector() * ((-MovementSpeed * 0.65f)* DeltaTime));
	}
	else if (bHostile && !bAttack)
	{
	// If enemy is hostile, move towards player
	FVector Direction = Player->GetActorLocation() - GetActorLocation();
	// if enemy too close to player, stop moving

	if (Direction.Size() > 100)
	{
		Direction.Normalize();
		AddMovementInput(Direction, MovementSpeed * DeltaTime);
	}


	}
	else if (bStunned) {
		// If enemy is stunned, do nothing
		FVector Direction = Player->GetActorLocation() + GetActorLocation();
		Direction.Normalize();
		AddMovementInput(Direction, MovementSpeed * DeltaTime);
	}
	else {
		// If enemy is not hostile, do nothing
	}
	RotateTowardsPlayer();

	// Raycast to see if player is in sight
	FHitResult HitResult;
	FVector Start = GetActorLocation();
	FVector End = Start + (GetActorForwardVector() * 1000.f);
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams);
	if (bHit)
	{
		if (HitResult.GetActor()->IsA(APlayerCharacter::StaticClass()))
		{
			bHostile = true;
			// print string
		}
		else
		{
			bHostile = false;

		}
	}
	
	
}

void ARadishEnemy::UnbindDelegatesAndDestroy()
{
	// Unbind all delegates before destroying
	CollisionComponent->OnComponentBeginOverlap.RemoveDynamic(this, &ARadishEnemy::OnOverlapBegin);
	SightSphere->OnComponentBeginOverlap.RemoveDynamic(this, &ARadishEnemy::OnTriggerBegin);
	SightSphere->OnComponentEndOverlap.RemoveDynamic(this, &ARadishEnemy::OnTriggerEnd);
	AttackRange->OnComponentBeginOverlap.RemoveDynamic(this, &ARadishEnemy::OnAttackRangeOverlapBegin);

	Destroy();
}


void ARadishEnemy::RotateTowardsPlayer()
{
	FVector Direction = Player->GetActorLocation() - GetActorLocation();
	Direction.Normalize();
	const FRotator NewLookAt = FRotationMatrix::MakeFromX(Direction).Rotator();
	SetActorRotation(NewLookAt);
}
	


void ARadishEnemy::SetStunned()
{
	bStunned = false;
}

void ARadishEnemy::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor == nullptr || OtherComp == nullptr || OtherActor == this)
	{
		return;
	}
	
	if (OtherComp->ComponentHasTag(FName("PlayerAttack")))
	{
		if(Player->IsComponentBehindWall(CollisionComponent))
		{
			return;		
		}
		TakeDamage(1, FDamageEvent(), nullptr, this);
		bStunned = true;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Ow"));

		GetWorldTimerManager().SetTimer(StunTimerHandle, this, &ARadishEnemy::SetStunned, 0.3f, false, 0.3f);
	}
}




void ARadishEnemy::OnTriggerBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor == nullptr || OtherComp == nullptr || OtherActor == this)
	{
		return;
	}

	// Enemy becomes hostile, will move towards player
	if (OtherComp->GetCollisionProfileName() == TEXT("Player"))
	{
		bHostile = true;
	}
}



void ARadishEnemy::OnTriggerEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(OtherActor == nullptr || OtherComp == nullptr || OtherActor == this)
	{
		return;
	}

	
	if (OtherComp->GetCollisionProfileName() == TEXT("Player"))
	{
	//	bHostile = false;
	}
}

void ARadishEnemy::OnAttackRangeOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor == nullptr || OtherComp == nullptr || OtherActor == this)
	{
		return;
	}
	
	if (OtherActor->IsA(APlayerCharacter::StaticClass()) && !bAttackDelayActive)
	{
		GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &ARadishEnemy::CheckIfStillOverlapping, 0.1f, false, AttackDelayTime);
	}
}

void ARadishEnemy::CheckIfStillOverlapping()
{
	// Checks if enemy attack range component is still overlapping with player, if so, attack again, with a delay of half a second
	if (AttackRange->IsOverlappingActor(Player) && !bAttackDelayActive) 
	{
		if(bIsDead)
		{
			return;
		}
		bAttack = true;
		bAttackDelayActive = true;
		GetWorldTimerManager().SetTimer(AttackDelayTimerHandle, this, &ARadishEnemy::SetAttackDelayBool, 0.1f, false, 0.5f);
	}
	else 
	{
		bAttack = false;
	}
}

void ARadishEnemy::SetAttackDelayBool() 
{
	bAttackDelayActive = false;
}

void ARadishEnemy::ReducePlayerHealth()
{
	GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &ARadishEnemy::CheckIfStillOverlapping, 0.1f, false, AttackDelayTime);
}