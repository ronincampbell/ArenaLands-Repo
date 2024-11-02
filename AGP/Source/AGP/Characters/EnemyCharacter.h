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

	friend class UEnemySquadSubsystem;

public:
	// Sets default values for this character's properties
	AEnemyCharacter();

	// To be called when an explosion goes off close to the enemy
	void OnHearExplosion(const FVector& ExplosionLocation);

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
	 * A pointer to the Enemy Squad Subsystem.
	 */
	UPROPERTY()
	UEnemySquadSubsystem* EnemySquadSubsystem;
	
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

	// The delay, in seconds, before the enemy fully detects the player if sight line isn't broken
	UPROPERTY(EditAnywhere)
	float DetectionDelay = 1.0f;
	// The delay, in seconds, before the enemy returns to idle states after losing sight of player during combat
	UPROPERTY(EditAnywhere)
	float ReturnToIdleDelay = 5.0f;
	float DetectionTimer = 0.0f;
	// Whether the enemy has fully detected the player
	bool DetectedPlayer = false;

	// The time, in seconds, that the enemy will wait at the last location they saw the player before returning to idle states
	UPROPERTY(EditAnywhere)
	float InvestigateTimeout = 6.0f;
	float InvestigateTimer = 0.0f;
	// How close the enemy has to be to the node they're investigating before the InvestigateTimer starts
	UPROPERTY(EditAnywhere)
	float InvestigateError = 100.0f;

	// How long, in seconds, the enemy will scatter for before returning to combat
	UPROPERTY(EditAnywhere)
	float ScatterTimeout = 2.0f;
	float ScatterTimer = 0.0f;

	// The last place the enemy saw the player, so the enemy doesn't lose track of the player as soon as they go around a corner
	FVector LastSeenPlayerLocation {};
	// The last place the enemy heard an explosion from, used to scatter away from the location
	FVector LastExplosiveLocation {};
	float LastSeenPlayerHealth = 1.0f;

	// The enemy's ID within their squad, used to determine positioning when guarding and patrolling
	UPROPERTY(VisibleAnywhere)
	int SquadID = -1;
	// The index of the enemy's squad, used to pull information from the Enemy Squad Subsystem
	UPROPERTY(VisibleAnywhere)
	int SquadIndex = -1;

	// Value used to measure enemy's confidence in combat and control behaviours
	UPROPERTY(VisibleAnywhere)
	float CurrentMorale = 0.0f;
	// Value added to morale based on own health ratio
	UPROPERTY(EditAnywhere)
	float HealthMoraleContribution = 3.0f; 
	// Value added to morale per squad mate
	UPROPERTY(EditAnywhere)
	float SquadMateMoraleContribution = 1.5f; 
	// Value subtracted from morale based on player health ratio
	UPROPERTY(EditAnywhere)
	float PlayerHealthMoraleContribution = 2.0f;
	// Function to recalculate CurrentMorale based on contributing factors
	void UpdateMorale();
	// Below this morale value, the enemy retreats during combat
	UPROPERTY(EditAnywhere)
	float MoraleRetreatCutoff = 1.0f; 
	// Above this morale value, the enemy pushes forward during combat
	UPROPERTY(EditAnywhere)
	float MoralePushCutoff = 3.0f; 

	// Determines whether enemy uses Guard or Patrol state when idle
	bool HasPatrolDuty();
	// Whether the enemy considers itself to be in cover (relative to the player)
	UPROPERTY()
	bool bInCover = false;
	// How far away from their treasure the enemy should consider when looking for cover
	UPROPERTY(EditAnywhere)
	float CoverCheckDistance = 200.0f;
	// The offset from the center of the enemy to their feet
	FVector FootOffset = FVector(0.0f, 0.0f, -88.0f);
	// The offset from the player's position to their head
	FVector PlayerHeadOffset = FVector(0.0f, 0.0f, 60.0f);
	// The offset from the ground to the point used when checking for cover
	UPROPERTY(EditAnywhere)
	FVector CoverCheckOffset = FVector(0.0f,0.0f,15.0f);
	// Function to recalculate bInCover based on current player position
	void UpdateCover();
	// Shortest time enemy can spend standing from cover to fire
	UPROPERTY(EditAnywhere)
	float MinimumPopupDuration = 0.5f;
	// Longest time enemy can spend standing from cover to fire
	UPROPERTY(EditAnywhere)
	float MaximumPopupDuration = 1.5f;
	// Shortest time enemy can spend crouching behind cover
	UPROPERTY(EditAnywhere)
	float MinimumHideDuration = 2.0f;
	float CoverTimer = 0.0f;

	// Whether the enemy has a treasure to protect
	bool bHasTreasure = false;
	void OnSquadDisbanded();
	// Radius around Treasure to patrol
	UPROPERTY(EditAnywhere)
	float TerritoryRadius = 400.0f; 

	// Position enemy will move to when in guard state
	UPROPERTY()
	FVector GuardLocation {};
	// Distance from the treasure to select guard positions from
	UPROPERTY(EditAnywhere)
	float GuardRadius = 100.0f;

	// Debug option, makes enemy print their CurrentState to the Output Log
	UPROPERTY(EditAnywhere)
	bool bPrintState = false;

	// Moves the enemy into one of the combat states, based on their current morale
	void EnterCombat();
	// Moves the enemy into one of the idle states, based on if their treasure is secure and what their duty is
	void EnterIdle();
	// Whether the enemy is in a combat state (ie. Push, Hold or Retreat)
	bool IsInCombat() const;
	// Whether the enemy is in an idle state (ie. Guard, Patrol or Wander)
	bool IsIdle() const;
	// Function called by SendCallouts() to send information about detected player
	void ReceiveCallout(APlayerCharacter* SensedPlayer);
	// Sends information about detected player to SquadMates
	void SendCallouts();
	// Finds the nearest node to StartLocation that is considered cover
	FVector FindNearestCoverLocation(const FVector& StartLocation) const;
	/**
	 * Reassigns the squad ID of this enemy. Used after a SquadMate has died.
	 * Also updates patrol duty and guard positions to redistribute load across squad
	 */
	void ReassignSquadID();
	/**
	 * Determines whether it is safe to fire at FireLocation
	 * A shot is unsafe if a perfectly accurate shot towards the location would hit another enemy
	 * @param FireLocation The location to fire at
	 * @return Whether it is safe to fire at FireLocation
	 */
	bool IsSafeToFire(const FVector& FireLocation) const;

public:	

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	
	/**
	 * NOT USED ANYMORE - Was used for TickEvade and TickEngage before we setup the UPawnSensingComponent.
	 * @return A pointer to one APlayerCharacter actor in the world.
	 */
	APlayerCharacter* FindPlayer() const;

	// Gets the current position of the player or, if the player isn't within sight, the last position they were seen at
	FVector GetCurrOrLastPlayerPos() const;

};
