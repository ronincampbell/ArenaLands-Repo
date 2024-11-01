// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponComponent.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

class UHealthComponent;

UCLASS()
class AGP_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();

	UFUNCTION(BlueprintCallable)
	bool HasWeapon();

	void EquipWeapon(bool bEquipWeapon, const FWeaponStats& WeaponStats = FWeaponStats());
	UFUNCTION(BlueprintImplementableEvent)
	void EquipWeaponGraphical(bool bEquipWeapon);

	/**
	 * Will reload the weapon if the character has a weapon equipped.
	 */
	void Reload();

	// Public bools for each modification so we can use them for models in the blueprint
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	bool bHasFireRateMod = false;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	bool bHasBaseDamageMod = false;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	bool bHasMagazineSizeMod = false;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	bool bHasReloadTimeMod = false;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	bool bIsExplosive = false;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	bool bIsShotgun = false;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FString ActiveMods = "Equipped Mods: \n";

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/**
	 * A scene component to store the position that hit scan shots start from. For the enemy character this could
	 * be placed close to the guns position for example and for the player character it can be placed close to the
	 * camera position.
	 */
	UPROPERTY(VisibleAnywhere)
	USceneComponent* BulletStartPosition;

	/**
	 * A component that holds information about the health of the character. This component has functions
	 * for damaging the character and healing the character.
	 */
	UPROPERTY(VisibleAnywhere)
	UHealthComponent* HealthComponent;

	/**
	 * An actor component that controls the logic for this characters equipped weapon.
	 */
	UPROPERTY(Replicated)
	UWeaponComponent* WeaponComponent = nullptr;

	/**
	 * Will fire at a specific location and handles the impact of the shot such as determining what it hit and
	 * deducting health if it hit a particular type of actor.
	 * @param FireAtLocation The location that you want to fire at.
	 * @return true if a shot was taken and false otherwise.
	 */
	void Fire(const FVector& FireAtLocation);

	// Updates modification bools based on weapon stats
	void UpdateModBools(const FWeaponStats& WeaponStats);

private:
	void EquipWeaponImplementation(bool bEquipWeapon, const FWeaponStats& WeaponStats = FWeaponStats());

	UFUNCTION(NetMulticast, Reliable)
	void MulticastEquipWeapon(bool bEquipWeapon);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
