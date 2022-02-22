// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"

#include "TentacleBossClass.generated.h"

UCLASS(Blueprintable)
class FRUITCAKEPROJECT_API ATentacleBossClass : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATentacleBossClass();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		void DealDamageMethod(float damage_amount);

protected:
	// Box collision component.
	UPROPERTY(VisibleDefaultsOnly, Category = BossNum1)
		UBoxComponent* LeftWeakPoints[6];

	// Box collision component.
	UPROPERTY(VisibleDefaultsOnly, Category = BossNum1)
		UBoxComponent* RightWeakPoints[6];

	// Boss Health
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float m_BossNum1_Health_Points = 1.f;
	float m_BossNum1_Max_Health = 1.f;
};
