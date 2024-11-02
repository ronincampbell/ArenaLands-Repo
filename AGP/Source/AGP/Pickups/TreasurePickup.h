// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickupBase.h"
#include "WeaponPickup.h"
#include "AGP/Characters/WeaponComponent.h"
#include "TreasurePickup.generated.h"

UCLASS()
class AGP_API ATreasurePickup : public APickupBase
{
	GENERATED_BODY()
	friend class UEnemySquadSubsystem;

protected:

	/**
	 * A pointer to the Enemy Squad Subsystem.
	 */
	UPROPERTY()
	UEnemySquadSubsystem* EnemySquadSubsystem;
	
	virtual void BeginPlay() override;
	virtual void OnPickupOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& HitInfo) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EWeaponRarity WeaponRarity = EWeaponRarity::Common;
	UPROPERTY(EditAnywhere)
	int NumEnemies = 1;
	UPROPERTY(EditAnywhere)
	float SquadSpawnRadius = 400.0f;
	
	FWeaponStats WeaponStats;

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateTreasurePickupMaterial();

	int SquadIndex = -1;
	
private:
	void GenerateTreasurePickup();

	TArray<EWeaponModification> GenerateWeaponModifications(int32 NumModifications);
	void ApplyWeaponModifications(const TArray<EWeaponModification>& Modifications);
};
