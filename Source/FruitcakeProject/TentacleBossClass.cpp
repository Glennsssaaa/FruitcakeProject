// Fill out your copyright notice in the Description page of Project Settings.


#include "TentacleBossClass.h"

// Sets default values
ATentacleBossClass::ATentacleBossClass()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!LeftWeakPoints[0])
	{
	//	// Set Collsion box to be sphere.
	//	LeftWeakPoints[0] = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftCollision"));
	//	// Set collision box radius.
	//	LeftWeakPoints[0]->SetBoxExtent(FVector(16.f, 64.f, 64.f));
	//	LeftWeakPoints[0]->SetRelativeLocation(FVector(10.f, 10.f, 10.f));
	//	// Set the root component to be newly created component.

	////	LeftWeakPoints[0]->BodyInstance.SetCollisionProfileName(TEXT("Enemy"));

	}
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

