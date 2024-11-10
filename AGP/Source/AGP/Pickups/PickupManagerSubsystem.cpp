// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupManagerSubsystem.h"

#include "WeaponPickup.h"
#include "AGP/AGPGameInstance.h"
#include "AGP/Pathfinding/PathfindingSubsystem.h"



void UPickupManagerSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// We don't want this pickup manager to do any spawning if it isn't
	// on the server.
	// A value < NM_Client is any type of server. So if it is >=
	// to NM_Client or == NM_Client then we know it is the client
	// and we don't want to spawn.
	if (GetWorld()->GetNetMode() >= NM_Client)
	{
		return;
	}

	if (PossibleSpawnLocations.IsEmpty())
	{
		PopulateSpawnLocations();
	}

	TimeSinceLastSpawn += DeltaTime;
	if (TimeSinceLastSpawn >= PickupSpawnRate)
	{
		SpawnWeaponPickup();
		TimeSinceLastSpawn = 0.0f;
	}
}

void UPickupManagerSubsystem::SpawnWeaponPickup()
{
	if (PossibleSpawnLocations.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("Unable to spawn weapon pickup."))
		return;
	}
	
	if (const UAGPGameInstance* GameInstance =
		GetWorld()->GetGameInstance<UAGPGameInstance>())
	{
		const int32 Index = FMath::RandRange(0, PossibleSpawnLocations.Num()-1);
		FVector SpawnPosition = PossibleSpawnLocations[Index];
		SpawnPosition.Z += 50.0f;

		// If the map contains a pickup at this index key then destroy it.
		// Otherwise get the map ready to store the about to be spawned pickup.
		if (SpawnedPickups.Contains(Index))
		{
			// Destroy the WeaponPickup at that location in the Map if one already exists.
			GetWorld()->DestroyActor(SpawnedPickups[Index]);
			SpawnedPickups[Index] = nullptr;
		} else
		{
			// If it doesn't exist, then add a section to the map that includes this Index position.
			SpawnedPickups.Add(Index, nullptr);
		}

		// Now we are here, we know that there is a free slot at the given Index key position to spawn a new
		// weapon pickup.
		SpawnedPickups[Index] = GetWorld()->SpawnActor<AWeaponPickup>(
			GameInstance->GetWeaponPickupClass(), SpawnPosition, FRotator::ZeroRotator);
	}
}

void UPickupManagerSubsystem::PickupConsumed(APickupBase* Pickup)
{
	int32 IndexToRemove = -1;
	// Find the pickup in the map
	for (auto IndexPickupPair : SpawnedPickups)
	{
		if (IndexPickupPair.Value == Pickup)
		{
			GetWorld()->DestroyActor(IndexPickupPair.Value);
			IndexToRemove = IndexPickupPair.Key;
			break;
		}
	}
	// Some fail state that occurs if it can't find the weapon pickup.
	if (IndexToRemove == -1)
	{
		UE_LOG(LogTemp, Error, TEXT("Couldn't find weapon pickup in the SpawnedPickup map. This shouldn't happen."));
		return;
	}

	SpawnedPickups.Remove(IndexToRemove);
}

void UPickupManagerSubsystem::PopulateSpawnLocations()
{
	PossibleSpawnLocations.Empty();
	if (const UPathfindingSubsystem* PathfindingSubsystem = GetWorld()->GetSubsystem<UPathfindingSubsystem>())
	{
		PossibleSpawnLocations = PathfindingSubsystem->GetWaypointPositions();
	}
}
