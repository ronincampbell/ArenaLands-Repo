// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponPickup.h"

#include "PickupManagerSubsystem.h"
#include "../Characters/PlayerCharacter.h"
#include "Net/UnrealNetwork.h"

void AWeaponPickup::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_Authority)
	{
		GenerateWeaponPickup();
	}
	UpdateWeaponPickupMaterial();
}

void AWeaponPickup::OnPickupOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& HitInfo)
{
	//Super::OnPickupOverlap(OverlappedComponent, OtherActor, OtherComponent, OtherBodyIndex, bFromSweep, HitInfo);
	// UE_LOG(LogTemp, Display, TEXT("Overlap event occurred on WeaponPickup"))

	if (ABaseCharacter* Player = Cast<ABaseCharacter>(OtherActor))
	{
		Player->EquipWeapon(true, WeaponStats);
		// We only want to delete it in the authority version.
		if (GetLocalRole() == ROLE_Authority)
		{
			// Get the PickupManager subsystem.
			if (UPickupManagerSubsystem* PickupManager = GetWorld()->GetSubsystem<UPickupManagerSubsystem>())
			{
				PickupManager->PickupConsumed(this);
			}
		}
	}
}

void AWeaponPickup::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWeaponPickup, WeaponRarity);
	DOREPLIFETIME(AWeaponPickup, WeaponStats);
}

void AWeaponPickup::GenerateWeaponPickup()
{
	WeaponRarity = WeaponRarityPicker();
	
	int32 NumModifications = 1; // Common has 1 slot
	switch (WeaponRarity)
	{
	case EWeaponRarity::Legendary:
		NumModifications = 4;
		break;
	case EWeaponRarity::Master:
		NumModifications = 3;
		break;
	case EWeaponRarity::Rare:
		NumModifications = 2;
		break;
	default:
		NumModifications = 1;
		break;
	}

	TArray<EWeaponModification> Modifications = GenerateWeaponModifications(NumModifications);
	ApplyWeaponModifications(Modifications);
	UE_LOG(LogTemp, Warning, TEXT("Generated Weapon: \n%s"), *WeaponStats.ToString())
}

TArray<EWeaponModification> AWeaponPickup::GenerateWeaponModifications(int32 NumModifications)
{
	TArray<EWeaponModification> Modifications;
	TArray<EWeaponModification> PossibleMods = {EWeaponModification::FireRate, EWeaponModification::BaseDamage, EWeaponModification::MagazineSize, EWeaponModification::ReloadTime, EWeaponModification::IsExplosive, EWeaponModification::IsShotgun};

	for (int32 i = 0; i < NumModifications; i++)
	{
		const int32 RandIndex = FMath::RandRange(0, PossibleMods.Num() - 1);
		Modifications.Add(PossibleMods[RandIndex]);
		if(PossibleMods[RandIndex] == EWeaponModification::IsExplosive || PossibleMods[RandIndex] == EWeaponModification::IsShotgun)
		{
			PossibleMods.RemoveAt(RandIndex);
		}
	}

	return Modifications;
}

void AWeaponPickup::ApplyWeaponModifications(const TArray<EWeaponModification>& Modifications)
{
	for (EWeaponModification Mod : Modifications)
	{
		switch (Mod)
		{
		case EWeaponModification::FireRate:
			WeaponStats.FireRate *= 0.8f; // Decrease fire rate delay by 20%
			//UE_LOG(LogTemp, Display, TEXT("Fire Rate Mod Equipped"));
			break;
		case EWeaponModification::BaseDamage:
			WeaponStats.BaseDamage *= 1.2f; // Increase base damage by 20%
			//UE_LOG(LogTemp, Display, TEXT("Base Damage Mod Equipped"));
			break;
		case EWeaponModification::MagazineSize:
			WeaponStats.MagazineSize *= 2; // Double magazine size
			//UE_LOG(LogTemp, Display, TEXT("Magazine Size Mod Equipped"));
			break;
		case EWeaponModification::ReloadTime:
			WeaponStats.ReloadTime *= 0.7f; // Decrease reload time by 30%
			//UE_LOG(LogTemp, Display, TEXT("Reload Time Mod Equipped"));
			break;
		case EWeaponModification::IsExplosive:
			WeaponStats.IsExplosive = true;
			WeaponStats.BaseDamage *= 0.5f;
			break;
		case EWeaponModification::IsShotgun:
			WeaponStats.IsShotgun = true;
			WeaponStats.Accuracy *= 0.92f;
			break;
		}
	}
}

EWeaponRarity AWeaponPickup::WeaponRarityPicker()
{
	// Rules:
	// 50% chance of Common
	// 30% chance of Rare
	// 15% chance of Master
	// 5% chance of Legendary
	const float RandPercent = FMath::RandRange(0.0f, 1.0f);
	
	if (RandPercent <= 0.5f)
	{
		return EWeaponRarity::Common;
	}
	if (RandPercent <= 0.8f)
	{
		return EWeaponRarity::Rare;
	}
	if (RandPercent <= 0.95f)
	{
		return EWeaponRarity::Master;
	}
	
	return EWeaponRarity::Legendary;
}
