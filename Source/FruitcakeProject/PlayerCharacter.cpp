// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "Math/UnrealMathUtility.h" 
#include "Components/InputComponent.h" 
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

}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	m_Can_Move = true;
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
//	SetActorRotation(GetActorRotation() += FRotator(1.f));

	if (!UKismetMathLibrary::NearlyEqual_FloatFloat(m_Rotation_Angle, m_Target_Angle, 0.5f))
	{
		float Rotation_Step = m_Rotation_Speed * DeltaTime;
		if (m_Target_Angle < m_Rotation_Angle)
		{
			Rotation_Step *= -1;
		}

		m_Rotation_Angle += Rotation_Step;

		FVector New_Offset = FVector(cosf(UKismetMathLibrary::DegreesToRadians(m_Rotation_Angle)) * 1000.f, sinf(UKismetMathLibrary::DegreesToRadians(m_Rotation_Angle)) * 1000.f, 1000.f);
		CameraBoom->TargetOffset = New_Offset;
		CameraBoom->SetRelativeRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation() + New_Offset, GetActorLocation()));
		//cameraBoom.setRelativeRotation(FindLookAtRotation(cameraBoom.GetSocketLocation + New_Offset, GetActorLocation()));
	}
	else
	{
		m_Rotation_Angle = m_Target_Angle;
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
//	PlayerInputComponent->BindAxis("TurnRate", this, &APlayerCharacter::TurnAtRateMethod);
//	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &APlayerCharacter::LookUpRateMethod);

	// PERSPECTIVE SWITCHING 
	PlayerInputComponent->BindAxis("SwitchPerspective", this, &APlayerCharacter::SwitchPerspectiveMethod);

	/* ---------- ACTION MAPPINGS ----------*/

	// DASHING
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &APlayerCharacter::DashInputMethod);

	// AOE test
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APlayerCharacter::FireAoeAtPlayer);
	PlayerInputComponent->BindAction("FireBig", IE_Pressed, this, &APlayerCharacter::FireABiggerAoe);

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

	// launch character forward
	LaunchCharacter(direction * 1000, false, true);

	// set timer, this is how long before the player can dash again (Half a second)
	GetWorldTimerManager().SetTimer(DashTimerHandle, this, &APlayerCharacter::ResetDashMethod, .25f, false);
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
	}
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

		FVector SpawnLocation;
		FRotator CameraRotation;
		//	GetActorEyesViewPoint(CameraLocation, CameraRotation);


		SpawnLocation = FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z );
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
			AProjectiles* AOECircle = World->SpawnActor<AProjectiles>(ProjectileClass, SpawnLocation, FRotator(0.f, 0.f, 0.f), SpawnParams);

			if (AOECircle)
			{
				// Set the projectile's initial trajectory.
				FVector LaunchDirection = MuzzleRotation.Vector();
				AOECircle->FireInDirection(LaunchDirection, 1.f);
				// remove energy
			}

		}
	}
}

void APlayerCharacter::ReducePlayerHealth()
{
	m_Player_Health_Points -= 0.1f;
}