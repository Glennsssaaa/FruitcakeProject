// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// Set default health value and set health value to default
	DefaultHealth = 100.f;
	health = DefaultHealth;
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	health = DefaultHealth;
	AActor* Owner = GetOwner();
	if(Owner)
	{
		Owner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::TakeDamage);
	}
}

void UHealthComponent::TakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamagedCauser)
{
	// if(Damage <= 0)
	// {
	// 	return;
	// }
	

	// Prevent health from overflowing (dealing negative damage healths the player)
	if(health - Damage > DefaultHealth)
	{
		health = DefaultHealth;
		return;
	}

	// Prevent health from going below 0
	if (health - Damage <= 0)
	{
		health = 0;
		return;
	}

	// Otherwise apply damage as normal
	health -= Damage;

	if(health <= 0.f)
	{
		if(!GetOwner()->ActorHasTag("Player"))
		{
			// GetOwner()->Destroy();
		}
		else
		{
			// end game state
		}
	}	
}




