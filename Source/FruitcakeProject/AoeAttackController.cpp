// Fill out your copyright notice in the Description page of Project Settings.

#include "AoeAttackController.h"
#include "Engine/DecalActor.h"
#include "Components/DecalComponent.h"
#include "Components/SphereComponent.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
AAoeAttackController::AAoeAttackController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	// Set up AOE Collision Sphere
	if(!AoeCollisionSphere)
	{
		AoeCollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AoeCollisionSphere"));
		AoeCollisionSphere->SetSphereRadius(256.f);
		AoeCollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		AoeCollisionSphere->SetCollisionProfileName(TEXT("EnemyAOE"));
		RootComponent = AoeCollisionSphere;
	}

	// Set up AOE Decal Material
	if(!AoeDecalMaterial)
	{
		AoeDecalMaterial = ConstructorHelpers::FObjectFinder<UMaterial>(TEXT("Material'/Game/Fruitcake_Game/Enemies/EnemyAssets/Mushroom/AOEDecal.AOEDecal'")).Object;
	}
	
	// Set up AOE Decal
	if(!AoeDecal)
	{
		AoeDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("AoeDecal"));
		AoeDecal->DecalSize = FVector(128.f, 256.f, 256.f);
		AoeDecal->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
		// Assign Decal Material
		AoeDecal->SetDecalMaterial(AoeDecalMaterial);
		AoeDecal->SetupAttachment(RootComponent);
	}
}

// Called when the game starts or when spawned
void AAoeAttackController::BeginPlay()
{
	Super::BeginPlay();
	AoeLifeSpan = 3.f;
	// Set timer until aoe attack is destroyed
	GetWorld()->GetTimerManager().SetTimer(AoeTimerHandle, this, &AAoeAttackController::ActivateAoe, AoeLifeSpan, false);
}

void AAoeAttackController::ActivateAoe()
{
	if(AoeCollisionSphere->IsOverlappingActor(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->TakeDamage(1.f, FDamageEvent(), nullptr, nullptr);
		Destroy();
	}
}




