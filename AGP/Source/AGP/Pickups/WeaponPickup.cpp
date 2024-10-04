#include "WeaponPickup.h"
#include "../Characters/PlayerCharacter.h"

void AWeaponPickup::BeginPlay()
{
	Super::BeginPlay();
	
	GenerateWeaponPickup();
	UpdateWeaponPickupMaterial();
}

void AWeaponPickup::OnPickupOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& HitInfo)
{
	if (ABaseCharacter* Player = Cast<ABaseCharacter>(OtherActor))
	{
		Player->EquipWeapon(true, WeaponStats);
		Destroy();
	}
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
}

TArray<EWeaponModification> AWeaponPickup::GenerateWeaponModifications(int32 NumModifications)
{
	TArray<EWeaponModification> Modifications;
	TArray<EWeaponModification> PossibleMods = {EWeaponModification::FireRate, EWeaponModification::BaseDamage, EWeaponModification::MagazineSize, EWeaponModification::ReloadTime};

	for (int32 i = 0; i < NumModifications; i++)
	{
		const int32 RandIndex = FMath::RandRange(0, PossibleMods.Num() - 1);
		Modifications.Add(PossibleMods[RandIndex]);
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
		}
	}
}

EWeaponRarity AWeaponPickup::WeaponRarityPicker()
{
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
