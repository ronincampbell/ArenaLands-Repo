// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"

#include "EnemyCharacter.h"
#include "HealthComponent.h"
#include "PlayerCharacter.h"
#include "AGP/MultiplayerGameMode.h"
#include "Net/UnrealNetwork.h"

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

void ABaseCharacter::Fire(const FVector& FireAtLocation)
{
	if (HasWeapon())
	{
		WeaponComponent->Fire(BulletStartPosition->GetComponentLocation(), FireAtLocation);
	}
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

	if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(this))
	{
		PlayerCharacter->UpdateModificationDetails(ActiveMods);
	}
}

void ABaseCharacter::Reload()
{
	if (HasWeapon())
	{
		WeaponComponent->Reload();
	}
}

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABaseCharacter, WeaponComponent);

	DOREPLIFETIME(ABaseCharacter, bHasFireRateMod)
	DOREPLIFETIME(ABaseCharacter, bHasBaseDamageMod)
	DOREPLIFETIME(ABaseCharacter, bHasMagazineSizeMod)
	DOREPLIFETIME(ABaseCharacter, bHasReloadTimeMod)
	DOREPLIFETIME(ABaseCharacter, bIsExplosive)
	DOREPLIFETIME(ABaseCharacter, bIsShotgun)
}

void ABaseCharacter::OnDeath()
{
	// WE ONLY WANT TO HANDLE LOGIC IF IT IS ON THE SERVER
	if (GetLocalRole() != ROLE_Authority) return;

	// IT IS PROBABLY BETTER PRACTICE TO INCLUDE THE PLAYER CHARACTER AND ENEMY CHARACTER DEATH LOGIC IN THEIR
	// OWN CLASSES INSTEAD OF HANDLING IT ON THE BASE CHARACTER (my bad...)
	if (AMultiplayerGameMode* GameMode = Cast<AMultiplayerGameMode>(GetWorld()->GetAuthGameMode()))
	{
		if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(this))
		{
			// Tell the GameMode to respawn this player.
			//GameMode->RespawnPlayer(GetController());
			PlayerCharacter->UpdateGameOverVisibility(true);
		}

		if (AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(this))
		{
			EnemyCharacter->OnEnemyDeath();
			//GameMode->RespawnEnemy(EnemyCharacter);
		}
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
	if (GetLocalRole() == ROLE_Authority)
	{
		EquipWeaponImplementation(bEquipWeapon, WeaponStats);
		MulticastEquipWeapon(bEquipWeapon, WeaponStats);
	}
}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ABaseCharacter::EquipWeaponImplementation(bool bEquipWeapon, const FWeaponStats& WeaponStats)
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
	}

	UpdateModBools(WeaponStats);

	if (bEquipWeapon)
	{
		UE_LOG(LogTemp, Display, TEXT("Player has equipped weapon."))
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Player has unequipped weapon."))
	}
}

void ABaseCharacter::MulticastEquipWeapon_Implementation(bool bEquipWeapon, const FWeaponStats& WeaponStats)
{
	//EquipWeaponImplementation(bEquipWeapon, WeaponStats);
	EquipWeaponGraphical(bEquipWeapon);
}

