// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
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
	
	// Camera Boom Set Up
	if (!CameraBoom)
	{
		CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
		CameraBoom->TargetArmLength = 0.f;
		CameraBoom->bInheritPitch = false;
		CameraBoom->bInheritRoll = false;
		CameraBoom->bInheritYaw = false;
		
		CameraBoom->SetupAttachment(RootComponent);
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

	// PROJECTILE FIRING
	PlayerInputComponent->BindAction("CastProjectile", IE_Pressed, this, &APlayerCharacter::CastProjectileMethod);

}



// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Set Character Rotation Rate (How fast the player rotates to the direction they are moving)
	GetCharacterMovement()->RotationRate = FRotator(2160.f, 2160.f, 2160.f);
	m_Can_Move = true;
	
	// Dash Distance and Speed
	m_Dash_Distance = 600.f;
	m_Dash_Speed = 6.f;
	bCan_Dash = true;

	// Melee Attacks
	can_Damage = true;
	is_Attack = false;

	// Spell Attacks
	can_Cast = true;

	// Player look and turn rate
	m_Look_Rate = 45.f;
	m_Turn_Rate = 45.f;

	// Camera 
	m_Camera_Zoom_Value = 1050.f;
	CameraBoom->CameraLagSpeed = 5.f;
}



// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!UKismetMathLibrary::NearlyEqual_FloatFloat(m_Rotation_Angle, m_Target_Angle, 100.f * DeltaTime))
	{
		CameraBoom->bEnableCameraLag = false;
		float Rotation_Step = m_Rotation_Speed * DeltaTime;
		if (m_Target_Angle < m_Rotation_Angle)
		{
			Rotation_Step *= -1;
		}

		m_Rotation_Angle += Rotation_Step;

		//Vector(X = cos(angle) * radius, Y = sin(angle) * radius, Z = height)
		const FVector New_Offset = FVector(cosf(UKismetMathLibrary::DegreesToRadians(m_Rotation_Angle)) * m_Camera_Zoom_Value, sinf(UKismetMathLibrary::DegreesToRadians(m_Rotation_Angle)) * m_Camera_Zoom_Value, m_Camera_Zoom_Value);
		CameraBoom->TargetOffset = New_Offset;
		CameraBoom->SetRelativeRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation() + New_Offset, GetActorLocation()));
	}
	else
	{
		m_Rotation_Angle = m_Target_Angle;
		if (!is_Dashing) 
		{
			CameraBoom->bEnableCameraLag = true;
		}
	}
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
		if (m_Can_Move)
		{
			const FRotator Rotation = m_Cam_Rotate;
			const FRotator Yaw(0, Rotation.Yaw, 0);

			// gets forward vector
			const FVector direction = FRotationMatrix(Yaw).GetUnitAxis(EAxis::X);

			AddMovementInput(direction, Value);
		}
		moveDir = Value;
	}
	else
	{
		moveDir = 0;
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
		if (m_Can_Move)
		{
			const FRotator Rotation = m_Cam_Rotate;
			const FRotator Yaw(0, Rotation.Yaw, 0);

			// gets right vector
			const FVector direction = FRotationMatrix(Yaw).GetUnitAxis(EAxis::Y);

			AddMovementInput(direction, Value);
		}
	}
}

void APlayerCharacter::TurnAtRateMethod(float Value)
{
	if(Value == 0)
	{
		return;
	}
	
	AddControllerYawInput(Value *= GetWorld()->GetDeltaSeconds() * m_Turn_Rate);
}

void APlayerCharacter::LookUpRateMethod(float Value)
{
	if(Value == 0)
	{
		return;
	}
	
	AddControllerPitchInput(Value *= GetWorld()->GetDeltaSeconds() * m_Look_Rate);
}

void APlayerCharacter::SwitchPerspectiveMethod(float value)
{
	// add axis value (E = 90, Q = -90) to target angle
	if (UKismetMathLibrary::NearlyEqual_FloatFloat(m_Rotation_Angle, m_Target_Angle, 0.1f))
	{
		m_Target_Angle += value;
	}
	m_Cam_Rotate = FRotator::ZeroRotator;
	m_Cam_Rotate.Roll = Camera->GetComponentRotation().Roll;
}

void APlayerCharacter::CastProjectileMethod()
{
	if (can_Cast)
	{
		// Set MuzzleOffset to spawn projectiles slightly in front of the camera.
		MuzzleOffset.Set(100.0f, 0.0f, 0.0f);

		const FVector SpawnLocation = FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + 50);

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
				// Set the projectile's initial trajectory.
				const FVector LaunchDirection = MuzzleRotation.Vector();
				Projectile->FireInDirection(LaunchDirection, false, true);
				can_Cast = false;
				GetWorldTimerManager().SetTimer(SpellcastTimerHandle, this, &APlayerCharacter::ResetProjectile, .2f, false);
			}

		}
	}
}

void APlayerCharacter::ResetProjectile()
{
	can_Cast = true;
}


bool APlayerCharacter::ImprovedDashFunction()
{
	// get forward vector
	CameraBoom->bEnableCameraLag = false;
	const FRotator Rotation = GetActorRotation();
	const FRotator Yaw(0, Rotation.Yaw, 0);
	const FVector direction = FRotationMatrix(Yaw).GetUnitAxis(EAxis::X);
	is_Dashing = true;
	m_Predicted_Location = (direction * m_Dash_Distance) + m_Base_Location;

	FHitResult SweepHitResult;

	// set actor location using interpolation and check if there is any collision in the way
	SetActorLocation(FMath::VInterpTo(GetActorLocation(), m_Predicted_Location, GetWorld()->GetDeltaSeconds(), (m_Dash_Speed * GetWorld()->GetDeltaSeconds()) * 50), true, &SweepHitResult);
	
	const FVector2D ActorLocation2D = FVector2D(GetActorLocation().X, GetActorLocation().Y);
	const FVector2D PredictedLocation2D = FVector2D(m_Predicted_Location.X, m_Predicted_Location.Y);

	if (SweepHitResult.bBlockingHit || ActorLocation2D.Equals(PredictedLocation2D, 100.f))
	{
		EnableInput(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		GetWorld()->GetTimerManager().ClearTimer(t_Dash_Function);
		CameraBoom->bEnableCameraLag = true;
		is_Dashing = false;
		GetWorld()->GetTimerManager().SetTimer(t_Dash_Cooldown_Timer, this, &APlayerCharacter::DashCooldown, m_Dash_Cooldown, false);
		return true;
	}
	return false;
}

void APlayerCharacter::DashCooldown()
{
	bCan_Dash = true;
}


void APlayerCharacter::RotatePlayerToCursor()
{
}

bool APlayerCharacter::GetCanDamage()
{
	return can_Damage;
}

void APlayerCharacter::SetCanDamage()
{
	can_Damage = false;
}



