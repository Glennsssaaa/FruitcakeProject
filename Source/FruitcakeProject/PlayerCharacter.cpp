// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "Math/UnrealMathUtility.h" 
#include "Components/InputComponent.h" 
#include "Components/CapsuleComponent.h"
#include "AoeAttackController.h"
#include "Projectiles.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// stuff that you should do
	m_Look_Rate = 45.f;
	m_Turn_Rate = 45.f;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0, 540.f, 0);

	// Set player health and energy values to max (1.f = 100%)
	m_Player_Health_Points = 1.f;
	m_Player_Energy_Points = 1.f; // may not be used, just leftover from 302

	// Set experience points to 0 (1.f = 100%, meaning a level up, the experience will then reset to 0), may not be used, just leftover from 302

	m_Player_Experience_Points = 0.f;

	AOEAttackClass = AAoeAttackController::StaticClass();
	ProjectileClass = AProjectiles::StaticClass();

	if (!CameraBoom)
	{
		CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
		CameraBoom->TargetArmLength = 0.f;
		CameraBoom->bInheritPitch = false;
		CameraBoom->bInheritRoll = false;
		CameraBoom->bInheritYaw = false;

		CameraBoom->SetupAttachment(RootComponent);
	}

	if (!Camera)
	{
		Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

		Camera->SetupAttachment(CameraBoom);
	}

	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &APlayerCharacter::OnHit);

}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	m_Can_Move = true;
	can_Cast = true;
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//	SetActorRotation(GetActorRotation() += FRotator(1.f));

	if (!UKismetMathLibrary::NearlyEqual_FloatFloat(m_Rotation_Angle, m_Target_Angle, 2.5f))
	{
		float Rotation_Step = m_Rotation_Speed * DeltaTime;
		if (!m_Rotate_Up)
		{
			Rotation_Step *= -1;
		}

		m_Rotation_Angle += Rotation_Step;

		//check for overflow
		if (m_Rotation_Angle < 0)
		{
			m_Rotation_Angle = 360 + m_Rotation_Angle;
		}
		if (m_Rotation_Angle > 360)
		{
			m_Rotation_Angle = (int)m_Rotation_Angle % 360;
		}

		//Vector(X = cos(angle) * radius, Y = sin(angle) * radius, Z = height)
		FVector New_Offset = FVector(cosf(UKismetMathLibrary::DegreesToRadians(m_Rotation_Angle)) * 1200.f, sinf(UKismetMathLibrary::DegreesToRadians(m_Rotation_Angle)) * 1200.f, 1200.f);
		CameraBoom->TargetOffset = New_Offset;
		CameraBoom->SetRelativeRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation() + New_Offset, GetActorLocation()));
	}
	else
	{
		m_Rotation_Angle = m_Target_Angle;
	}

	GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Red, FString::Printf(TEXT("Current: %.2f    Target: %.2f"), m_Rotation_Angle, m_Target_Angle));
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
//	PlayerInputComponent->BindAxis("TurnRate", this, &APlayerCharacter::TurnAtRateMethod);
//	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &APlayerCharacter::LookUpRateMethod);

	/* ---------- ACTION MAPPINGS ----------*/

	// DASHING
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &APlayerCharacter::DashInputMethod);

	// AOE test
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APlayerCharacter::FireAoeAtPlayer);
	PlayerInputComponent->BindAction("FireBig", IE_Pressed, this, &APlayerCharacter::FireABiggerAoe);

	// PERSPECTIVE SWITCHING 
//	PlayerInputComponent->BindAxis("SwitchPerspective", this, &APlayerCharacter::SwitchPerspectiveMethod);

	// PROJECTILE FIRING
	PlayerInputComponent->BindAction("CastProjectile", IE_Pressed, this, &APlayerCharacter::CastProjectileMethod);

}




void APlayerCharacter::MoveForwardMethod(float value)
{
	// W and S Movement
	if (Controller != NULL && value != 0)
	{
		if (m_Can_Move)
		{
			const FRotator Rotation = m_Cam_Rotate;
			const FRotator Yaw(0, Rotation.Yaw, 0);

			// gets forward vector
			const FVector direction = FRotationMatrix(Yaw).GetUnitAxis(EAxis::X);

			AddMovementInput(direction, value);
		}
	}
}

void APlayerCharacter::MoveRightMethod(float value)
{
	// A and D movement
	if (Controller != NULL && value != 0)
	{
		if (m_Can_Move)
		{
			const FRotator Rotation = m_Cam_Rotate;
			const FRotator Yaw(0, Rotation.Yaw, 0);

			// gets right vector
			const FVector direction = FRotationMatrix(Yaw).GetUnitAxis(EAxis::Y);

			AddMovementInput(direction, value);
		}
	}
}

void APlayerCharacter::TurnAtRateMethod(float value)
{
	AddControllerYawInput(value *= GetWorld()->GetDeltaSeconds() * m_Turn_Rate);
}

void APlayerCharacter::LookUpRateMethod(float value)
{
	AddControllerPitchInput(value *= GetWorld()->GetDeltaSeconds() * m_Look_Rate);
}


void APlayerCharacter::DashInputMethod()
{
	if (Controller != NULL && !is_Dashing)
	{
		is_Dashing = true;
		// remove energy
		m_Player_Energy_Points -= 0.05f;

		// shoot player up first to prevent collision with floor during dash
		//LaunchCharacter(FVector(0.f, 0.f, 300.f), false, false);

		// set a timer before next function to prevent forward movement from happening too soon
		GetWorldTimerManager().SetTimer(DashTimerHandle, this, &APlayerCharacter::DashMethod, 0.1f, false);
	}
}

void APlayerCharacter::DashMethod()
{
	// get forward vector
	const FRotator Rotation = GetActorRotation();
	const FRotator Yaw(0, Rotation.Yaw, 0);
	const FVector direction = FRotationMatrix(Yaw).GetUnitAxis(EAxis::X);


	//disable friction while dashing
	GetCharacterMovement()->BrakingDecelerationWalking = 0.f;
	GetCharacterMovement()->BrakingFrictionFactor = 0.f;

	// set timer, this is how long before the player can dash again (Half a second)
	GetWorldTimerManager().SetTimer(DashTimerHandle, this, &APlayerCharacter::ResetDashMethod, .5f, false);
	// launch character forward
	LaunchCharacter(direction * 1000, false, true);
}

void APlayerCharacter::ResetDashMethod()
{
	is_Dashing = false;

	//enable friction when dash stops
	GetCharacterMovement()->BrakingDecelerationWalking = 2048.f;
	GetCharacterMovement()->BrakingFrictionFactor = 2.f;
}

void APlayerCharacter::SwitchPerspectiveMethod(float value)
{
	// add axis value (E = 90, Q = -90) to target angle
	if (UKismetMathLibrary::NearlyEqual_FloatFloat(m_Rotation_Angle, m_Target_Angle, 0.1f))
	{
		m_Target_Angle += value;
		if (value > 0)
		{
			m_Rotate_Up = true;
		}
		else
		{
			m_Rotate_Up = false;
		}

		//check for overflow
		if (m_Target_Angle < 0)
		{
			m_Target_Angle = 360 + m_Target_Angle;
		}
		if (m_Target_Angle > 360)
		{
			m_Target_Angle = (int)m_Target_Angle % 360;
		}
	}
	m_Cam_Rotate = FRotator::ZeroRotator;
	m_Cam_Rotate.Roll = Camera->GetComponentRotation().Roll;
}

void APlayerCharacter::CastProjectileMethod()
{
	if (can_Cast)
	{
		FVector CameraLocation;
		FRotator CameraRotation;
		GetActorEyesViewPoint(CameraLocation, CameraRotation);

		// Set MuzzleOffset to spawn projectiles slightly in front of the camera.
		MuzzleOffset.Set(100.0f, 0.0f, 0.0f);

		// Transform MuzzleOffset from camera space to world space.
		FVector MuzzleLocation = CameraLocation + FTransform(CameraRotation).TransformVector(MuzzleOffset);

		FVector SpawnLocation = FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + 50);

		// set rotation of projectile to camera rotation
		FRotator MuzzleRotation = GetActorRotation();

		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();

			// Spawn the projectile at the muzzle.
			AProjectiles* Projectile = World->SpawnActor<AProjectiles>(ProjectileClass, SpawnLocation, MuzzleRotation, SpawnParams);

			if (Projectile)
			{
				// Set the projectile's initial trajectory.
				FVector LaunchDirection = MuzzleRotation.Vector();
				Projectile->FireInDirection(LaunchDirection, false, true);
				can_Cast = false;
				GetWorldTimerManager().SetTimer(ProjectileTimerHandle, this, &APlayerCharacter::ResetProjecitle, .2f, false);
			}

		}
	}
}

void APlayerCharacter::ResetProjecitle()
{
	can_Cast = true;
}


void APlayerCharacter::RotatePlayerToCursor()
{
	FHitResult test;
	FVector MouseLocation;
	FVector MouseDirection;

	APlayerController* PController = GetWorld()->GetFirstPlayerController();


	PController->DeprojectMousePositionToWorld(MouseLocation, MouseDirection);

	FRotator charRotation = GetActorRotation();
	FRotator targetRotation = MouseDirection.Rotation();

	FRotator newRot = FRotator(charRotation.Pitch, targetRotation.Yaw, charRotation.Roll);


	FRotator PlayerRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), MouseLocation);

	SetActorRelativeRotation(newRot);

}

void APlayerCharacter::FireAoeAtPlayer()
{
	// ensure aoe class is initialised

	if (AOEAttackClass)
	{

		FVector SpawnLocation;
		FRotator CameraRotation;
		//	GetActorEyesViewPoint(CameraLocation, CameraRotation);


		SpawnLocation = FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z - 100);
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
			AAoeAttackController* AOECircle = World->SpawnActor<AAoeAttackController>(AOEAttackClass, SpawnLocation, FRotator(0.f, 0.f, 0.f), SpawnParams);

			if (AOECircle)
			{
				// Set the projectile's initial trajectory.
				FVector LaunchDirection = MuzzleRotation.Vector();
				AOECircle->FireAtLocation(LaunchDirection, 1.f);
				// remove energy
			}

		}
	}
}

void APlayerCharacter::FireABiggerAoe()
{
	if (ProjectileClass)
	{

		//	FVector SpawnLocation;
		//	FRotator CameraRotation;
		//	//	GetActorEyesViewPoint(CameraLocation, CameraRotation);


		//	SpawnLocation = FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z );
		//	// Set MuzzleOffset to spawn projectiles slightly in front of the camera.
		//	MuzzleOffset.Set(100.0f, 40.0f, 0.0f);

		//	// Transform MuzzleOffset from camera space to world space.
		////	FVector MuzzleLocation = CameraLocation + FTransform(CameraRotation).TransformVector(MuzzleOffset);

		//	// set rotation of projectile to camera rotation
		//	FRotator MuzzleRotation = CameraRotation;

		//	UWorld* World = GetWorld();
		//	if (World)
		//	{
		//		FActorSpawnParameters SpawnParams;
		//		SpawnParams.Owner = this;
		//		SpawnParams.Instigator = GetInstigator();

		//		// Spawn the projectile at the muzzle.
		//		AProjectiles* AOECircle = World->SpawnActor<AProjectiles>(ProjectileClass, SpawnLocation, FRotator(0.f, 0.f, 0.f), SpawnParams);

		//		if (AOECircle)
		//		{
		//			// Set the projectile's initial trajectory.
		//			FVector LaunchDirection = MuzzleRotation.Vector();
		//			AOECircle->FireInDirection(LaunchDirection, 1.f);
		//			// remove energy
		//		}

		//	}
	}
}

void APlayerCharacter::ReducePlayerHealth()
{
	m_Player_Health_Points -= 0.1f;
}

void APlayerCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (is_Dashing && Cast<UStaticMeshComponent>(OtherComp) != NULL)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Red, TEXT("hit"));
	}
}

void APlayerCharacter::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (is_Dashing && Cast<UStaticMeshComponent>(OtherComponent) != NULL)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Red, TEXT("hit"));

		// set timer, this is how long before the player can dash again (Half a second)
		GetWorldTimerManager().SetTimer(DashTimerHandle, this, &APlayerCharacter::ResetDashMethod, .25f, false);
	}
}