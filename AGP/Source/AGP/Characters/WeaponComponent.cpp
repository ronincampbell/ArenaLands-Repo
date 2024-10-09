#include "WeaponComponent.h"
#include "BaseCharacter.h"
#include "HealthComponent.h"

// Sets default values for this component's properties
UWeaponComponent::UWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

bool UWeaponComponent::Fire(const FVector& BulletStart, const FVector& FireAtLocation)
{
	// Check if we can fire
	if (TimeSinceLastShot < WeaponStats.FireRate || IsMagazineEmpty())
	{
		return false;
	}

	// Check if shotgun mode is enabled
	if (WeaponStats.IsShotgun)
	{
		// Shotgun mode: Fire all remaining ammo
		int32 NumProjectiles = RoundsRemainingInMagazine;

		for (int32 i = 0; i < NumProjectiles; i++)
		{
			// Apply random spread to each projectile
			FVector RandomFireAt = FMath::VRand();
			float CurrentShotDistance = FVector::Distance(BulletStart, FireAtLocation);
			RandomFireAt *= CurrentShotDistance;
			RandomFireAt += BulletStart;
			FVector AccuracyAdjustedFireAt = FMath::Lerp(RandomFireAt, FireAtLocation, FMath::RandRange(0.8f, WeaponStats.Accuracy));

			FHitResult HitResult;
			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(GetOwner());

			if (!WeaponStats.IsExplosive)
			{
				// Standard Shotgun Projectile Logic
				if (GetWorld()->LineTraceSingleByChannel(HitResult, BulletStart, AccuracyAdjustedFireAt, ECC_WorldStatic, QueryParams))
				{
					if (ABaseCharacter* HitCharacter = Cast<ABaseCharacter>(HitResult.GetActor()))
					{
						if (UHealthComponent* HitCharacterHealth = HitCharacter->GetComponentByClass<UHealthComponent>())
						{
							HitCharacterHealth->ApplyDamage(WeaponStats.BaseDamage);
						}
					}
					DrawDebugLine(GetWorld(), BulletStart, HitResult.ImpactPoint, FColor::Green, false, 1.0f);
				}
				else
				{
					DrawDebugLine(GetWorld(), BulletStart, AccuracyAdjustedFireAt, FColor::Red, false, 1.0f);
				}
			}
			else
			{
				// Shotgun with Explosive logic
				if (GetWorld()->LineTraceSingleByChannel(HitResult, BulletStart, AccuracyAdjustedFireAt, ECC_WorldStatic, QueryParams))
				{
					// Explosive hit something
					DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, WeaponStats.ExplosionRadius, 12, FColor::Red, false, 1.0f);

					// Apply radial damage in a sphere around the impact point
					TArray<FHitResult> HitResults;
					FCollisionShape ExplosionSphere = FCollisionShape::MakeSphere(WeaponStats.ExplosionRadius);

					if (GetWorld()->SweepMultiByChannel(HitResults, HitResult.ImpactPoint, HitResult.ImpactPoint, FQuat::Identity, ECC_WorldStatic, ExplosionSphere, QueryParams))
					{
						for (auto& ExplosionHit : HitResults)
						{
							if (ABaseCharacter* AffectedCharacter = Cast<ABaseCharacter>(ExplosionHit.GetActor()))
							{
								if (UHealthComponent* AffectedHealth = AffectedCharacter->GetComponentByClass<UHealthComponent>())
								{
									AffectedHealth->ApplyDamage(WeaponStats.BaseDamage);
								}
							}
						}
					}
				}
			}
		}

		// Decrease magazine to zero after all shots fired
		RoundsRemainingInMagazine = 0;
	}
	else
	{
		// Standard firing logic
		FVector RandomFireAt = FMath::VRand();
		float CurrentShotDistance = FVector::Distance(BulletStart, FireAtLocation);
		RandomFireAt *= CurrentShotDistance;
		RandomFireAt += BulletStart;
		FVector AccuracyAdjustedFireAt = FMath::Lerp(RandomFireAt, FireAtLocation, WeaponStats.Accuracy);

		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(GetOwner());

		if (!WeaponStats.IsExplosive)
		{
			// Standard weapon logic
			if (GetWorld()->LineTraceSingleByChannel(HitResult, BulletStart, AccuracyAdjustedFireAt, ECC_WorldStatic, QueryParams))
			{
				if (ABaseCharacter* HitCharacter = Cast<ABaseCharacter>(HitResult.GetActor()))
				{
					if (UHealthComponent* HitCharacterHealth = HitCharacter->GetComponentByClass<UHealthComponent>())
					{
						HitCharacterHealth->ApplyDamage(WeaponStats.BaseDamage);
					}
				}
				DrawDebugLine(GetWorld(), BulletStart, HitResult.ImpactPoint, FColor::Green, false, 1.0f);
			}
			else
			{
				DrawDebugLine(GetWorld(), BulletStart, AccuracyAdjustedFireAt, FColor::Red, false, 1.0f);
			}
		}
		else
		{
			// Explosive weapon logic
			if (GetWorld()->LineTraceSingleByChannel(HitResult, BulletStart, AccuracyAdjustedFireAt, ECC_WorldStatic, QueryParams))
			{
				DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, WeaponStats.ExplosionRadius, 12, FColor::Red, false, 1.0f);

				TArray<FHitResult> HitResults;
				FCollisionShape ExplosionSphere = FCollisionShape::MakeSphere(WeaponStats.ExplosionRadius);

				if (GetWorld()->SweepMultiByChannel(HitResults, HitResult.ImpactPoint, HitResult.ImpactPoint, FQuat::Identity, ECC_WorldStatic, ExplosionSphere, QueryParams))
				{
					for (auto& ExplosionHit : HitResults)
					{
						if (ABaseCharacter* AffectedCharacter = Cast<ABaseCharacter>(ExplosionHit.GetActor()))
						{
							if (UHealthComponent* AffectedHealth = AffectedCharacter->GetComponentByClass<UHealthComponent>())
							{
								AffectedHealth->ApplyDamage(WeaponStats.BaseDamage);
							}
						}
					}
				}
			}
		}
		RoundsRemainingInMagazine--;
	}

	TimeSinceLastShot = 0.0f;
	return true;
}


void UWeaponComponent::Reload()
{
	// Shouldn't be able to reload if you are already reloading.
	if (bIsReloading) return;
	
	UE_LOG(LogTemp, Display, TEXT("Start Reload"))
	bIsReloading = true;
	UIBIsReloading = true;
}

void UWeaponComponent::CompleteReload()
{
	UE_LOG(LogTemp, Display, TEXT("Reload Complete"))
	RoundsRemainingInMagazine = WeaponStats.MagazineSize;
}

void UWeaponComponent::SetWeaponStats(const FWeaponStats& WeaponInfo)
{
	this->WeaponStats = WeaponInfo;
	// Set the number of bullets to the magazine size
	RoundsRemainingInMagazine = WeaponInfo.MagazineSize;

	UIReloadSpeed = WeaponStats.ReloadTime;
}

bool UWeaponComponent::IsMagazineEmpty()
{
	return RoundsRemainingInMagazine <= 0;
}

// Called when the game starts
void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

// Called every frame
void UWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	TimeSinceLastShot += DeltaTime;

	UIAmmoRemaining = RoundsRemainingInMagazine;
	UIMagazineSize = WeaponStats.MagazineSize;

	// Logic that delays the call to CompleteReload if the weapon is currently being reloaded.
	if (bIsReloading)
	{
		CurrentReloadDuration += DeltaTime;
		if (CurrentReloadDuration >= WeaponStats.ReloadTime)
		{
			bIsReloading = false;
			UIBIsReloading = false;
			CompleteReload();
			CurrentReloadDuration = 0.0f;
		}
	}
}
