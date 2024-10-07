#pragma once

#include "CoreMinimal.h"
#include "PickupBase.h"
#include "AGP/Characters/WeaponComponent.h"
#include "WeaponPickup.generated.h"

UENUM(BlueprintType)
enum class EWeaponRarity : uint8
{
	Common,
	Rare,
	Master,
	Legendary
};

UENUM(BlueprintType)
enum class EWeaponModification : uint8
{
	FireRate,
	BaseDamage,
	MagazineSize,
	ReloadTime,
	IsExplosive
};

UCLASS()
class AGP_API AWeaponPickup : public APickupBase
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadOnly)
	EWeaponRarity WeaponRarity = EWeaponRarity::Common;
	
	FWeaponStats WeaponStats;

	virtual void BeginPlay() override;
	virtual void OnPickupOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& HitInfo) override;

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateWeaponPickupMaterial();

private:
	void GenerateWeaponPickup();
	EWeaponRarity WeaponRarityPicker();
	TArray<EWeaponModification> GenerateWeaponModifications(int32 NumModifications);
	void ApplyWeaponModifications(const TArray<EWeaponModification>& Modifications);
};
