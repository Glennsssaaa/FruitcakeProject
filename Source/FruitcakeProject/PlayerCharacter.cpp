// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "DrawDebugHelpers.h"
#include "Math/UnrealMathUtility.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Math/Vector2D.h" 
#include "Components/InputComponent.h" 
#include "Projectiles.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0, 540.f, 0);
	
	SpellClass = AProjectiles::StaticClass();

	/*static ConstructorHelpers::FObjectFinder<UCameraShakeBase>Shake(TEXT(""));
	if (Shake.Succeeded())
	{
		ShakeClass = Shake.Object;
	}*/

	if(!Root)
	{
		Root = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	}
	
	// Camera Tracker Set Up
	if(!CameraTracker)
	{
		CameraTracker = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CameraTracker"));
		CameraTracker->SetupAttachment(RootComponent);
	}
	
	// Camera Boom Set Up
	if (!CameraBoom)
	{
		CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
		CameraBoom->TargetArmLength = 0.f;
		CameraBoom->bInheritPitch = false;
		CameraBoom->bInheritRoll = false;
		CameraBoom->bInheritYaw = false;
		
		CameraBoom->SetupAttachment(CameraTracker);
	}

	// Camera Setup
	if (!Camera)
	{
		Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
		Camera->SetupAttachment(CameraBoom);
	}
	
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	/* ---------- AXIS MAPPINGS ----------*/

	// WASD MOVEMENT
	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForwardMethod);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRightMethod);

	// PITCH, YAW, TURN AT AND LOOK AT RATES
	PlayerInputComponent->BindAxis("TurnRate", this, &APlayerCharacter::TurnAtRateMethod);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &APlayerCharacter::LookUpRateMethod);

	/* ---------- ACTION MAPPINGS ----------*/

	// DASHING
	//	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &APlayerCharacter::DashInputMethod);

	// PERSPECTIVE SWITCHING 
	PlayerInputComponent->BindAxis("SwitchPerspective", this, &APlayerCharacter::SwitchPerspectiveMethod);

	// MELEE ATTACK
	PlayerInputComponent->BindAction("Melee", IE_Pressed, this, &APlayerCharacter::MeleeAttack);
	
	// PROJECTILE FIRING
	PlayerInputComponent->BindAction("CastProjectile", IE_Pressed, this, &APlayerCharacter::CastProjectileMethod);

}



// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	SpellClass = AProjectiles::StaticClass();

	// Set Character Rotation Rate (How fast the player rotates to the direction they are moving)
	GetCharacterMovement()->RotationRate = FRotator(2160.f, 2160.f, 2160.f);
	bCanMove = true;
	
	// Dash Distance and Speed
	DashDistance = 600.f;
	DashSpeed = 1.33f;
	bCanDash = true;

	// Melee Attacks
	bCanDamage = true;
	bIsAttack = false;

	// Spell Attacks
	bCanCast = true;

	// Player look and turn rate
	LookRate = 45.f;
	TurnRate = 45.f;

	// Camera 
	CameraZoomValue = 1000.f;
	CameraBoom->CameraLagSpeed = 5.f;
	CameraBoom->TargetArmLength = 375.f;
	CameraBoom->bEnableCameraLag = true;

	perftest = true;

	SpellClass = AProjectiles::StaticClass();
}



// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!UKismetMathLibrary::NearlyEqual_FloatFloat(RotationAngle, TargetAngle, 100.f * DeltaTime))
    {
		CameraBoom->bEnableCameraLag = false;
        float Rotation_Step = RotationSpeed * DeltaTime;
        if (TargetAngle < RotationAngle)
        {
            Rotation_Step *= -1;
        }
   
        RotationAngle += Rotation_Step;
   
        //Vector(X = cos(angle) * radius, Y = sin(angle) * radius, Z = height)
        const FVector New_Offset = FVector(cosf(UKismetMathLibrary::DegreesToRadians(RotationAngle)) * CameraZoomValue, sinf(UKismetMathLibrary::DegreesToRadians(RotationAngle)) * CameraZoomValue, CameraZoomValue);
        CameraBoom->TargetOffset = New_Offset;
        CameraBoom->SetRelativeRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation() + New_Offset, GetActorLocation()));
    }
    else
    {
    	CameraBoom->bEnableCameraLag = true;
        RotationAngle = TargetAngle;
    }


}

bool APlayerCharacter::ImprovedDashFunction()
{
	// get forward vector
	bIsDashing = true;
	PredictedLocation = (GetActorForwardVector() * DashDistance) + BaseLocation;
	FHitResult SweepHitResult;
	// set actor location using interpolation and check if there is any collision in the way
//	SetActorLocation(FMath::VInterpTo(GetActorLocation(), PredictedLocation, GetWorld()->GetDeltaSeconds(), (DashSpeed)), true, &SweepHitResult);
	CameraBoom->bEnableCameraLag = false;

	SetActorLocation(FMath::Lerp(GetActorLocation(), PredictedLocation, GetWorld()->GetDeltaSeconds() * DashSpeed), true, &SweepHitResult);
	
	
	const FVector2D ActorLocation2D = FVector2D(GetActorLocation().X, GetActorLocation().Y);
	const FVector2D PredictedLocation2D = FVector2D(PredictedLocation.X, PredictedLocation.Y);
	if (SweepHitResult.bBlockingHit || ActorLocation2D.Equals(PredictedLocation2D, 100.f))
	{
		EnableInput(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		GetWorld()->GetTimerManager().ClearTimer(DashTimer);
		bIsDashing = false;
		GetWorld()->GetTimerManager().SetTimer(DashCooldownTimer, this, &APlayerCharacter::DashCooldownFunction, DashCooldown, false);
		CameraBoom->bEnableCameraLag = true;
		return true;
	}
	return false;
}

void APlayerCharacter::DashCooldownFunction()
{
	bCanDash = true;
}


void APlayerCharacter::MoveForwardMethod(float Value)
{
	// W and S Movement
	// check if controller is nullptr, if so, return instantly
	if(Controller == nullptr)
	{
		return;
	}
	
	if (Value != 0)
	{
		if (bCanMove)
		{
			const FRotator Rotation = CameraRotate;
			const FRotator Yaw(0, Rotation.Yaw, 0);

			// gets forward vector
			const FVector Direction = FRotationMatrix(Yaw).GetUnitAxis(EAxis::X);

			AddMovementInput(Direction, Value);
		}
		MoveDirection = Value;
	}
	else
	{
		MoveDirection = 0;
	}
}

void APlayerCharacter::MoveRightMethod(float Value)
{
	// A and D movement
	if(Controller == nullptr)
	{
		return;
	}
	
	if (Value != 0)
	{
		if (bCanMove)
		{
			const FRotator Rotation = CameraRotate;
			const FRotator Yaw(0, Rotation.Yaw, 0);

			// gets right vector
			const FVector Direction = FRotationMatrix(Yaw).GetUnitAxis(EAxis::Y);

			AddMovementInput(Direction, Value);
		}
	}
}

void APlayerCharacter::TurnAtRateMethod(float Value)
{
	if(Value == 0)
	{
		return;
	}
	
	AddControllerYawInput(Value *= GetWorld()->GetDeltaSeconds() * TurnRate);
}

void APlayerCharacter::LookUpRateMethod(float Value)
{
	if(Value == 0)
	{
		return;
	}
	
	AddControllerPitchInput(Value *= GetWorld()->GetDeltaSeconds() * LookRate);
}

void APlayerCharacter::SwitchPerspectiveMethod(float value)
{
	// add axis value (E = 90, Q = -90) to target angle
	if (UKismetMathLibrary::NearlyEqual_FloatFloat(RotationAngle, TargetAngle, 0.1f))
	{
		TargetAngle += value;
	}
	CameraRotate = FRotator::ZeroRotator;
	CameraRotate.Roll = Camera->GetComponentRotation().Roll;
}

void APlayerCharacter::MeleeAttack()
{
	// Check to make sure the player is able to move and if they are either currently dashing or attack, if so, immediately return from function
	if(!bCanMove || bIsAttack)
	{
		return;
	}

	//play camera shake
	GetWorld()->GetFirstPlayerController()->PlayerCameraManager->PlayWorldCameraShake(GetWorld(), ShakeClass, GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetTransform().GetLocation(), 0, 20, 1);

	bCanMove = false;
	bIsAttack = true;

	// Set amount of time before player can perform another melee attack
	MeleeCooldownTimer = 0.5f;

	// Get Forward Vector
	const FRotator Rotation = GetActorRotation();
	const FRotator Yaw(0, Rotation.Yaw, 0);
	const FVector Direction = FRotationMatrix(Yaw).GetUnitAxis(EAxis::X);
	
	// Launch Player Forward
	LaunchCharacter(Direction * 500, false, false);

	// Set Timer
	GetWorldTimerManager().SetTimer(MeleeTimerHandle, this, &APlayerCharacter::MeleeAttackCooldown, MeleeCooldownTimer, false);
}

void APlayerCharacter::MeleeAttackCooldown()
{
//	bCanAttack = true;
	bCanMove = true;
	bCanDamage = true;
	bIsAttack = false;
}

void APlayerCharacter::CastProjectileMethod()
{
	if (bCanCast)
	{
		const FVector SpawnLocation = FVector(ProjectileSpawnPoint.X, ProjectileSpawnPoint.Y, ProjectileSpawnPoint.Z);

		// set rotation of projectile to camera rotation
		const FRotator MuzzleRotation = GetActorRotation();

		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();

			// Spawn the projectile at the muzzle.
			AProjectiles* Projectile = World->SpawnActor<AProjectiles>(SpellClass, SpawnLocation, MuzzleRotation, SpawnParams);

			if (Projectile)
			{
				//play camera shake
				GetWorld()->GetFirstPlayerController()->PlayerCameraManager->PlayWorldCameraShake(GetWorld(), ShakeClass, GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetTransform().GetLocation(), 0, 20, 1);
				// Set the projectile's initial trajectory.
				const FVector LaunchDirection = ProjectileLaunchDirection.Vector();
				Projectile->FireInDirection(LaunchDirection, false, true);
				bCanCast = false;
				GetWorldTimerManager().SetTimer(CastTimerHandle, this, &APlayerCharacter::ResetProjectile, 1.f, false);
			}

		}
	}
}

void APlayerCharacter::CastProjectile()
{
	if(!bCanMove)
	{
		return;
	}

	
}

bool APlayerCharacter::IsComponentBehindWall(UShapeComponent* box)
{
	// Line trace to box
	FHitResult Hit;
	FVector Start = GetActorLocation();
	FVector End = box->GetComponentLocation();
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	if(GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, CollisionParams))
	{
		DrawDebugLine(GetWorld(), GetActorLocation(), box->GetComponentLocation(), FColor::Red, false, 1.0f, 0, 1.0f);
		// If the line trace hit something, return true
		if(Hit.GetComponent()->ComponentHasTag(FName(TEXT("MeleeCheckTag"))))
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("xtra good"));
			return false;
		}

		if (Hit.bBlockingHit)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("bad"));
			return true;
		}
	}
	// Draw debug lines
	DrawDebugLine(GetWorld(), GetActorLocation(), box->GetComponentLocation(), FColor::Red, false, 1.0f, 0, 1.0f);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("good"));
	return false;
}

void APlayerCharacter::ResetProjectile()
{
	bCanCast = true;
}




void APlayerCharacter::RotatePlayerToCursor()
{
}

bool APlayerCharacter::GetCanDamage()
{
	return bCanDamage;
}

void APlayerCharacter::SetCanDamage()
{
	bCanDamage = false;
}



