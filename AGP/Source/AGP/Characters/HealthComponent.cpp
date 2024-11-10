// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"

#include "PlayerCharacter.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
	// ...
}

bool UHealthComponent::IsDead()
{
	return bIsDead;
}

float UHealthComponent::GetCurrentHealth() const
{
	return CurrentHealth;
}

float UHealthComponent::GetCurrentHealthPercentage() const
{
	return CurrentHealth / MaxHealth;
}

void UHealthComponent::ApplyDamage(float DamageAmount)
{
	if (bIsDead) return;
	CurrentHealth -= DamageAmount;
	if (CurrentHealth <= 0.0f)
	{
		OnDeath();
		CurrentHealth = 0.0f;
	}
	UpdateHealthBar();
}

void UHealthComponent::ApplyHealing(float HealingAmount)
{
	if (bIsDead) return;
	CurrentHealth += HealingAmount;
	if (CurrentHealth > 100.0f)
	{
		CurrentHealth = 100.0f;
	}
	UpdateHealthBar();
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UHealthComponent, MaxHealth);
	DOREPLIFETIME(UHealthComponent, CurrentHealth);
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	CurrentHealth = MaxHealth;
}


void UHealthComponent::OnDeath()
{
	UE_LOG(LogTemp, Display, TEXT("The character has died."))
	bIsDead = true;
	// Tell the server base character that they have died.

	// This OnDeath function will only be called on the server in the current setup but it is still worth
	// checking that we are only handling this logic on the server.
	if (GetOwnerRole() != ROLE_Authority) return;
	
	if (ABaseCharacter* Character = Cast<ABaseCharacter>(GetOwner()))
	{
		Character->OnDeath();
	}
}

void UHealthComponent::UpdateHealthBar()
{
	if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetOwner()))
	{
		PlayerCharacter->UpdateHealthBar(GetCurrentHealthPercentage());
	}
}

void UHealthComponent::ResetHealth()
{
	UE_LOG(LogTemp, Display, TEXT("MAX HEALTH: %f"), MaxHealth)
	CurrentHealth = MaxHealth;
	bIsDead = false;
}

// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

