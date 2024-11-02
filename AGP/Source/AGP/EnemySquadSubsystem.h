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
	TMap<int, ATreasurePickup*> Treasures {};
	TMap<int, TArray<AEnemyCharacter*>> Squads {};

public:
	void RegisterTreasure(int SquadIndex, ATreasurePickup* Treasure);
	//int CreateSquad(ATreasurePickup* Treasure, int NumEnemies, float SquadSpawnRadius);
	int RegisterSquadMember(int SquadIndex, AEnemyCharacter* SquadMember);
	ATreasurePickup* GetTreasure(int SquadIndex);
	TArray<AEnemyCharacter*> GetSquadMembers(int SquadIndex);
	void DisbandSquad(int SquadIndex);
	void RemoveSquadMember(int SquadIndex, AEnemyCharacter* SquadMember);
};
