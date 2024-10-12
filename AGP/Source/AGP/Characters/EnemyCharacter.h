// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.h"
#include "PlayerCharacter.h"
#include "AGP/Pickups/TreasurePickup.h"
#include "EnemyCharacter.generated.h"

// Forward declarations to avoid needing to #include files in the header of this class.
// When these classes are used in the .cpp file, they are #included there.
class UPawnSensingComponent;
class APlayerCharacter;
class UPathfindingSubsystem;

/**
 * An enum to hold the current state of the enemy character.
 */
UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Guard,
	Patrol,
	Wander,
	Hold,
	Push,
	Retreat,
	Investigate,
	Scatter
};

/**
 * A class representing the logic for an AI controlled enemy character. 
 */
UCLASS()
class AGP_API AEnemyCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//UPROPERTY(VisibleAnywhere)
	//USceneComponent* CoverCheckPosition;
	/**
	 * Will move the character along the CurrentPath or do nothing to the character if the path is empty.
	 */
	void MoveAlongPath();

	/**
	 * Logic that controls the enemy character when in Idle states.
	 */
	void TickGuard();
	void TickPatrol();
	void TickWander();
	void TickInvestigate(float DeltaTime);
	/**
	 * Logic that controls the enemy character when in Combat states.
	 */
	void TickRetreat();
	void TickHold(float DeltaTime);
	void TickPush();
	void TickScatter(float DeltaTime);

	/**
	 * A function bound to the UPawnSensingComponent's OnSeePawn event. This will set the SensedCharacter variable
	 * if the pawn that was sensed was of type APlayerCharacter.
	 * @param SensedActor The pawn that was sensed by the UPawnSensingComponent.
	 */
	UFUNCTION()
	void OnSensedPawn(APawn* SensedActor);
	
	void OnHearExplosion(const FVector& ExplosionLocation);
	/**
	 * Will update the SensedCharacter variable based on whether the UPawnSensingComponent has a line of sight to the
	 * Player Character or not. This may cause the SensedCharacter variable to become a nullptr so be careful when using
	 * the SensedCharacter variable.
	 */
	void UpdateSight();

	/**
	 * A pointer to the Pathfinding Subsystem.
	 */
	UPROPERTY()
	UPathfindingSubsystem* PathfindingSubsystem;

	/**
	 * A pointer to the PawnSensingComponent attached to this enemy character.
	 */
	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensingComponent;

	/**
	 * A pointer to a PlayerCharacter that can be seen by this enemy character. If this is nullptr then the enemy cannot
	 * see any PlayerCharacter.
	 */
	UPROPERTY()
	APlayerCharacter* SensedCharacter = nullptr;
	//TODO: Add SensedExplosive variable once the explosives have been implemented

	/**
	 * An array of vectors representing the current path that the agent is traversing along.
	 */
	UPROPERTY(VisibleAnywhere)
	TArray<FVector> CurrentPath;

	/**
	 * The current state of the enemy character. This determines which logic to use when executing the finite state machine
	 * found in the tick function of this enemy character.
	 */
	UPROPERTY(EditAnywhere)
	EEnemyState CurrentState = EEnemyState::Guard;

	/**
	 * Some arbitrary error value for determining how close is close enough before moving onto the next step in the path.
	 */
	UPROPERTY(EditAnywhere)
	float PathfindingError = 150.0f; // 150 cm from target by default.
	
	UPROPERTY(EditAnywhere)
	float DetectionDelay = 1.0f;
	UPROPERTY(EditAnywhere)
	float ReturnToIdleDelay = 5.0f;
	float DetectionTimer = 0.0f;
	bool DetectedPlayer = false;
	
	UPROPERTY(EditAnywhere)
	float InvestigateTimeout = 6.0f;
	float InvestigateTimer = 0.0f;
	UPROPERTY(EditAnywhere)
	float InvestigateError = 100.0f;
	
	UPROPERTY(EditAnywhere)
	float ScatterTimeout = 2.0f;
	float ScatterTimer = 0.0f;
	
	FVector LastSeenPlayerLocation {};
	FVector LastExplosiveLocation {};
	float LastSeenPlayerHealth = 1.0f;

	UPROPERTY(EditAnywhere)
	TArray<AEnemyCharacter*> SquadMates;
	UPROPERTY(VisibleAnywhere)
	int SquadID = -1;

	//Value used to measure enemy's confidence in combat and control behaviours
	UPROPERTY(VisibleAnywhere)
	float CurrentMorale = 0.0f;
	//Value added based on own health ratio
	UPROPERTY(EditAnywhere)
	float HealthMoraleContribution = 3.0f; 
	//Value added to morale per squad mate
	UPROPERTY(EditAnywhere)
	float SquadMateMoraleContribution = 1.5f; 
	//Value subtracted based on player health ratio
	UPROPERTY(EditAnywhere)
	float PlayerHealthMoraleContribution = 2.0f; 
	void UpdateMorale();
	//Below this value, the enemy retreats during combat
	UPROPERTY(EditAnywhere)
	float MoraleRetreatCutoff = 1.0f; 
	//Above this value, the enemy pushes forward during combat
	UPROPERTY(EditAnywhere)
	float MoralePushCutoff = 3.0f; 

	//Determines whether enemy uses Guard or Patrol state when idle
	UPROPERTY(EditAnywhere)
	bool PatrolDuty = false; 
	UPROPERTY()
	bool InCover = false;
	UPROPERTY(EditAnywhere)
	float CoverCheckDistance = 200.0f;
	FVector FootOffset = FVector(0.0f, 0.0f, -88.0f);
	FVector PlayerHeadOffset = FVector(0.0f, 0.0f, 60.0f);
	UPROPERTY(EditAnywhere)
	FVector CoverCheckOffset = FVector(0.0f,0.0f,15.0f);
	void UpdateCover();
	//Longest time enemy can spend standing from cover to fire
	UPROPERTY(EditAnywhere)
	float MaximumPopupDuration = 1.5f;
	//Shortest time enemy can spend crouching behind cover
	UPROPERTY(EditAnywhere)
	float MinimumHideDuration = 2.0f;
	float CoverTimer = 0.0f;

	//Treasure that enemy is trying to protect, defines center of territory
	UPROPERTY(EditAnywhere)
	ATreasurePickup* Treasure = nullptr;
	bool HasTreasure();
	//Radius around Treasure to patrol and hold position within
	UPROPERTY(EditAnywhere)
	float TerritoryRadius = 400.0f; 

	UPROPERTY()
	FVector GuardLocation {};
	//How far enemy can be from their guard position during guard state
	UPROPERTY(EditAnywhere)
	float GuardError = 100.0f;
	//Distance from the treasure to select guard positions from
	UPROPERTY(EditAnywhere)
	float GuardRadius = 100.0f;

	UPROPERTY(EditAnywhere)
	bool bPrintState = false;

	void EnterCombat();
	void EnterIdle();
	bool IsInCombat() const;
	void ReceiveCallout(APlayerCharacter* SensedPlayer);
	void SendCallouts();
	FVector FindNearestCoverLocation(const FVector& StartLocation) const;
	void AddSquadMate(AEnemyCharacter* NewSquadMate);

public:	

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	
	/**
	 * NOT USED ANYMORE - Was used for TickEvade and TickEngage before we setup the UPawnSensingComponent.
	 * @return A pointer to one APlayerCharacter actor in the world.
	 */
	APlayerCharacter* FindPlayer() const;

	FVector GetCurrOrLastPlayerPos() const;

};
