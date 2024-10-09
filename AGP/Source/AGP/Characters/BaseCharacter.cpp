// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include "HealthComponent.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BulletStartPosition = CreateDefaultSubobject<USceneComponent>("Bullet Start");
	BulletStartPosition->SetupAttachment(GetRootComponent());
	HealthComponent = CreateDefaultSubobject<UHealthComponent>("Health Component");
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

bool ABaseCharacter::Fire(const FVector& FireAtLocation)
{
	if (HasWeapon())
	{
		return WeaponComponent->Fire(BulletStartPosition->GetComponentLocation(), FireAtLocation);
	}
	return false;
}

void ABaseCharacter::UpdateModBools(const FWeaponStats& WeaponStats)
{
	// Reset all the bools to false
	bHasFireRateMod = false;
	bHasBaseDamageMod = false;
	bHasMagazineSizeMod = false;
	bHasReloadTimeMod = false;
	bIsExplosive = false;
	bIsShotgun = false;
	ActiveMods = "Equipped Mods: \n";

	// Check for active modifications and set the corresponding bools to true
	if (WeaponStats.FireRate < 0.2f)
	{
		bHasFireRateMod = true;
		ActiveMods += "- Fire Rate Mod\n";
	}

	if (WeaponStats.BaseDamage > 10.0f)
	{
		bHasBaseDamageMod = true;
		ActiveMods += "- Base Damage Mod\n";
	}

	if (WeaponStats.MagazineSize > 5)
	{
		bHasMagazineSizeMod = true;
		ActiveMods += "- Magazine Size Mod\n";
	}

	if (WeaponStats.ReloadTime < 1.0f)
	{
		bHasReloadTimeMod = true;
		ActiveMods += "- Reload Time Mod\n";
	}

	if (WeaponStats.IsExplosive)
	{
		bIsExplosive = true;
		ActiveMods += "- Explosive Mod\n";
	}

	if (WeaponStats.IsShotgun)
    {
        bIsShotgun = true;
        ActiveMods += "- Shotgun Mod\n";
    }
}

void ABaseCharacter::Reload()
{
	if (HasWeapon())
	{
		WeaponComponent->Reload();
	}
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ABaseCharacter::HasWeapon()
{
	return (WeaponComponent != nullptr);
}

void ABaseCharacter::EquipWeapon(bool bEquipWeapon, const FWeaponStats& WeaponStats)
{
	// Create or remove the weapon component depending on whether we are trying to equip a weapon and we
	// don't already have one. Or if we are trying to unequip a weapon and we do have one.
	if (bEquipWeapon && !HasWeapon())
	{
		WeaponComponent = NewObject<UWeaponComponent>(this);
		WeaponComponent->RegisterComponent();
	}
	else if (!bEquipWeapon && HasWeapon())
	{
		WeaponComponent->UnregisterComponent();
		WeaponComponent = nullptr;
	}

	// At this point we should have a WeaponComponent if we are trying to equip a weapon.
	if (HasWeapon())
	{
		// Set the weapons stats to the given weapon stats.
		UE_LOG(LogTemp, Display, TEXT("Equipping weapon: \n%s"), *WeaponStats.ToString())
		WeaponComponent->SetWeaponStats(WeaponStats);
		UpdateModBools(WeaponStats);
	}
	
	EquipWeaponGraphical(bEquipWeapon);
	if (bEquipWeapon)
	{
		UE_LOG(LogTemp, Display, TEXT("Player has equipped weapon."))
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Player has unequipped weapon."))
	}
}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

