// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySquadSubsystem.h"

#include "AGPGameInstance.h"

int UEnemySquadSubsystem::CreateSquad(ATreasurePickup* Treasure, int NumEnemies, float SquadSpawnRadius)
{
	if (const UAGPGameInstance* GameInstance =
		GetWorld()->GetGameInstance<UAGPGameInstance>())
	{
		int SquadIndex = Squads.Num();
		Squads.Add(TArray<AEnemyCharacter*> {});
		Treasures.Add(Treasure);
		
		for(int i = 0; i < NumEnemies; ++i)
		{
			FVector SpawnPosition = Treasure->GetActorLocation() + FMath::VRand().GetSafeNormal2D()*SquadSpawnRadius;
			SpawnPosition.Z -= 40.0f;
			//FActorSpawnParameters SpawnParameters{};
			AEnemyCharacter* NewEnemy = GetWorld()->SpawnActor<AEnemyCharacter>(
				GameInstance->GetEnemyClass(), SpawnPosition, FRotator::ZeroRotator);

			Squads[SquadIndex].Add(NewEnemy);
			NewEnemy->SquadIndex = SquadIndex;
			NewEnemy->SquadID = i;
			NewEnemy->bHasTreasure = true;
			NewEnemy->EnterIdle();
		}

		return SquadIndex;
	}
	return -1;
}

ATreasurePickup* UEnemySquadSubsystem::GetTreasure(int SquadIndex)
{
	return Treasures[SquadIndex];
}

TArray<AEnemyCharacter*> UEnemySquadSubsystem::GetSquadMembers(int SquadIndex)
{
	return Squads[SquadIndex];
}

void UEnemySquadSubsystem::DisbandSquad(int SquadIndex)
{
	for(AEnemyCharacter* Enemy : Squads[SquadIndex])
	{
		Enemy->OnSquadDisbanded();
	}
	Squads.RemoveAt(SquadIndex);
	Treasures.RemoveAt(SquadIndex);
	for(int i = SquadIndex; i < Treasures.Num(); ++i)
	{
		for(AEnemyCharacter* Enemy : Squads[i])
		{
			Enemy->SquadIndex -= 1;
		}

		Treasures[i]->SquadIndex -= 1;
	}
}

void UEnemySquadSubsystem::RemoveSquadMember(int SquadIndex, AEnemyCharacter* SquadMember)
{
	int RemovedIndex = Squads[SquadIndex].Find(SquadMember);
	Squads[SquadIndex].RemoveAt(RemovedIndex);

	for(int i = RemovedIndex; i < Squads[SquadIndex].Num(); ++i)
	{
		Squads[SquadIndex][i]->SquadID -= 1;
	}
}
