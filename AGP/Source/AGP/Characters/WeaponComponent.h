#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponComponent.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8 {
	Rifle,
	Pistol
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
	int32 MagazineSize = 5;
	float ReloadTime = 1.0f;
	bool IsExplosive = false;
	bool IsShotgun = false;
	float ExplosionRadius = 200.0f;

	/**
	 * A debug ToString function that allows the easier printing of the weapon stats.
	 * @return A string detailing the weapon stats stored in this struct.
	 */
	FString ToString() const
	{
		FString WeaponString = "";
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

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int32 UIAmmoRemaining;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int32 UIMagazineSize;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	float UIReloadSpeed;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	bool UIBIsReloading;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	FWeaponStats WeaponStats;
	int32 RoundsRemainingInMagazine;
	float TimeSinceLastShot;
	
	bool bIsReloading = false;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	/**
	 * Called after the reload has been started delayed by the weapon stats reload time.
	 */
	void CompleteReload();
	float CurrentReloadDuration = 0.0f;
	const float ExplosionSoundRadiusMultiplier = 2.0f;

	bool FireImplementation(const FVector& BulletStart, const FVector& FireAtLocation, FVector& OutHitLocation);

	void FireVisualImplementation(const FVector& BulletStart, const FVector& HitLocation);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastFire(const FVector& BulletStart, const FVector& HitLocation);

	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector& BulletStart, const FVector& FireAtLocation);
};
