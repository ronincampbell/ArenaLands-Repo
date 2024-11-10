// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponComponent.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8 {
	Rifle,
	Pistol
};

UENUM(BlueprintType)
enum class EWeaponHitType : uint8
{
	NoHit,
	Character,
	Dirt
};

USTRUCT(BlueprintType)
struct FWeaponHitInfo
{
	GENERATED_BODY()

	UPROPERTY()
	FVector HitLocation;
	UPROPERTY()
	EWeaponHitType HitType;
};

USTRUCT(BlueprintType)
struct FWeaponStats
{
	GENERATED_BODY()
public:
	
	EWeaponType WeaponType = EWeaponType::Rifle;
	float Accuracy = 1.0f;
	float FireRate = 0.2f;
	float BaseDamage = 10.0f;
	UPROPERTY()
	int32 MagazineSize = 5;
	float ReloadTime = 1.0f;
	UPROPERTY()
	bool IsExplosive = false;
	bool IsShotgun = false;
	float ExplosionRadius = 200.0f;
	float ExplosionSoundMultiplier = 2.0f;
	/**
	 * A debug ToString function that allows the easier printing of the weapon stats.
	 * @return A string detailing the weapon stats stored in this struct.
	 */
	FString ToString() const
	{
		FString WeaponString = "";
		WeaponString += "Accuracy:      " + FString::SanitizeFloat(Accuracy) + "\n";
		WeaponString += "Fire Rate:     " + FString::SanitizeFloat(FireRate) + "\n";
		WeaponString += "Base Damage:   " + FString::SanitizeFloat(BaseDamage) + "\n";
		WeaponString += "Magazine Size: " + FString::FromInt(MagazineSize) + "\n";
		WeaponString += "Reload Time:   " + FString::SanitizeFloat(ReloadTime) + "\n";
		WeaponString += "Is Explosive:  " + FString(IsExplosive ? "True" : "False") + "\n";
		WeaponString += "Is Shotgun:    " + FString(IsShotgun ? "True" : "False");
		return WeaponString;
	}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AGP_API UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWeaponComponent();

	void Fire(const FVector& BulletStart, const FVector& FireAtLocation);
	/**
	 * Starts the process of reloading.
	 */
	void Reload();
	void SetWeaponStats(const FWeaponStats& WeaponInfo);

	bool IsMagazineEmpty();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(ReplicatedUsing=UpdateAmmoUI)
	FWeaponStats WeaponStats;
	UPROPERTY(ReplicatedUsing=UpdateAmmoUI)
	int32 RoundsRemainingInMagazine;
	float TimeSinceLastShot;
	bool bIsReloading = false;

	UFUNCTION()
	void UpdateAmmoUI();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	/**
	 * Called after the reload has been started delayed by the weapon stats reload time.
	 */
	void CompleteReload();
	float CurrentReloadDuration = 0.0f;

	bool FireImplementation(const FVector& BulletStart, const FVector& FireAtLocation, FWeaponHitInfo& HitInfo);
	void FireVisualImplementation(const FVector& BulletStart, const FWeaponHitInfo& HitInfo);
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastFire(const FVector& BulletStart, const FWeaponHitInfo& HitInfo);
	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector& BulletStart, const FVector& FireAtLocation);

	// RELOAD FUNCTIONS
	void ReloadImplementation();
	UFUNCTION(Server, Reliable)
	void ServerReload();
	
};
