// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AGP_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

	bool IsDead();
	float GetCurrentHealth() const;
	float GetCurrentHealthPercentage() const;
	void ApplyDamage(float DamageAmount);
	void ApplyHealing(float HealingAmount);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Replicated)
	float MaxHealth = 50.0f;
	UPROPERTY(VisibleAnywhere, Replicated)
	float CurrentHealth;
	bool bIsDead = false;

	void OnDeath();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
