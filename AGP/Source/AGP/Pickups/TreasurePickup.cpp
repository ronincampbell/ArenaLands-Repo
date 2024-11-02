// Fill out your copyright notice in the Description page of Project Settings.


#include "TreasurePickup.h"

#include "AGP/EnemySquadSubsystem.h"
#include "AGP/Characters/BaseCharacter.h"

void ATreasurePickup::BeginPlay()
{
	Super::BeginPlay();

	EnemySquadSubsystem = GetWorld()->GetSubsystem<UEnemySquadSubsystem>();
	
	GenerateTreasurePickup();
	UpdateTreasurePickupMaterial();

	if(SquadIndex >= 0)
	{
		EnemySquadSubsystem->RegisterTreasure(SquadIndex, this);
	}
}

void ATreasurePickup::OnPickupOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& HitInfo)
{
	if (ABaseCharacter* Player = Cast<ABaseCharacter>(OtherActor))
	{
		Player->EquipWeapon(true, WeaponStats);
		EnemySquadSubsystem->DisbandSquad(SquadIndex);
		Destroy();
	}
}

void ATreasurePickup::GenerateTreasurePickup()
{
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
}

TArray<EWeaponModification> ATreasurePickup::GenerateWeaponModifications(int32 NumModifications)
{
	TArray<EWeaponModification> Modifications;
	TArray<EWeaponModification> PossibleMods = {EWeaponModification::FireRate, EWeaponModification::BaseDamage, EWeaponModification::MagazineSize, EWeaponModification::ReloadTime, EWeaponModification::IsExplosive, EWeaponModification::IsShotgun};

	for (int32 i = 0; i < NumModifications; i++)
	{
		const int32 RandIndex = FMath::RandRange(0, PossibleMods.Num() - 1);
		Modifications.Add(PossibleMods[RandIndex]);
	}

	return Modifications;
}

void ATreasurePickup::ApplyWeaponModifications(const TArray<EWeaponModification>& Modifications)
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
			break;
		case EWeaponModification::IsShotgun:
			WeaponStats.IsShotgun = true;
			break;
		}
	}
}
