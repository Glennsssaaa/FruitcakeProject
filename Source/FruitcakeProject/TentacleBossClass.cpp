// Fill out your copyright notice in the Description page of Project Settings.


#include "TentacleBossClass.h"

// Sets default values
ATentacleBossClass::ATentacleBossClass()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	/* -------- Boss Collision Components Setup --------*/
	// collision component set up
	if (!FlowerCollisionComponent)
	{
		// Set Collsion box to be sphere.
		FlowerCollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("FlowerCollision"));
		FlowerCollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("nothing"));

		// Set collision box radius.
		FlowerCollisionComponent->SetBoxExtent(FVector(120.f, 120.f, 120.f));
		// Set the root component to be newly created component.
	//	RootComponent = FlowerCollisionComponent;
	}


	/* -------- Boss Meshes Setup --------*/

	// Using placeholder cube meshes 

	if (!FlowerMeshComponent)
	{
		// sets mesh of projectile to basic sphere mesh, loaded from unreal engine files
		FlowerMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComponent"));
		// placeholder cube mesh
		static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh(TEXT("StaticMesh'/Game/Geometry/Meshes/1M_Cube.1M_Cube'"));
		if (Mesh.Succeeded())
		{
			FlowerMeshComponent->SetStaticMesh(Mesh.Object);
		}
	}


	FlowerMeshComponent->SetupAttachment(RootComponent);
	FlowerMeshComponent->SetRelativeScale3D(FVector(2.f, 2.f, 2.f));

}

// Called when the game starts or when spawned
void ATentacleBossClass::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATentacleBossClass::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATentacleBossClass::DealDamageMethod(float damage_amount)
{
	if (m_BossNum1_Health_Points >= damage_amount)
	{
		m_BossNum1_Health_Points -= damage_amount;
	}
	else
	{
		m_BossNum1_Health_Points = 0.f;
	}
}

