// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/EnemyCharacter.h"
#include "Subsystems/WorldSubsystem.h"
#include "EnemySquadSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class AGP_API UEnemySquadSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<ATreasurePickup*> Treasures {};
	TArray<TArray<AEnemyCharacter*>> Squads {};

public:
	int CreateSquad(ATreasurePickup* Treasure, int NumEnemies, float SquadSpawnRadius);
	ATreasurePickup* GetTreasure(int SquadIndex);
	TArray<AEnemyCharacter*> GetSquadMembers(int SquadIndex);
	void DisbandSquad(int SquadIndex);
	void RemoveSquadMember(int SquadIndex, AEnemyCharacter* SquadMember);
};
