// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerGameMode.h"

#include "EngineUtils.h"
#include "Characters/PlayerCharacter.h"
#include "GameFramework/PlayerStart.h"

void AMultiplayerGameMode::RespawnAllPlayers()
{
	for (TActorIterator<APlayerController> It(GetWorld()); It; ++It)
	{
		RespawnPlayer(*It);
	}
}

void AMultiplayerGameMode::RespawnPlayer(AController* Controller)
{
	if (Controller)
	{
		if (APlayerCharacter* CurrentlyPossessedCharacter = Cast<APlayerCharacter>(Controller->GetPawn()))
		{
			Controller->UnPossess();
			CurrentlyPossessedCharacter->Destroy();
			RestartPlayer(Controller);
			if (APlayerCharacter* NewPossessedCharacter = Cast<APlayerCharacter>(Controller->GetPawn()))
			{
				NewPossessedCharacter->ChooseCharacterMesh();
				NewPossessedCharacter->DrawUI();
			}
		}
	}
	
}

void AMultiplayerGameMode::RespawnEnemy(AEnemyCharacter* EnemyCharacter)
{
	EnemyCharacter->Destroy();
	// Create a new enemy character at a random start position.
	// Because the EnemyCharacters aren't really set up with a Controller, we can't easily get a player start
	// location so I am going to iterate through all of the PLayer Start actors instead and pick one randomly.
	TArray<APlayerStart*> PlayerStartLocations;
	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
	{
		PlayerStartLocations.Add(*It);
	}
	if (PlayerStartLocations.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("The map doesn't contain any player start locations. Can't respawn enemy."));
		return;
	}
	APlayerStart* RandomStart = PlayerStartLocations[FMath::RandRange(0, PlayerStartLocations.Num()-1)];

	GetWorld()->SpawnActor<AEnemyCharacter>(EnemyCharacterClass, RandomStart->GetActorLocation(), RandomStart->GetActorRotation());
}
