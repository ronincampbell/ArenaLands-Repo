// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/EnemyCharacter.h"
#include "GameFramework/GameMode.h"
#include "MultiplayerGameMode.generated.h"

class APlayerCharacter;
/**
 * 
 */
UCLASS()
class AGP_API AMultiplayerGameMode : public AGameMode
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void RespawnAllPlayers();
	void RespawnPlayer(AController* Controller);
	void RespawnEnemy(AEnemyCharacter* EnemyCharacter);

protected:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<APlayerCharacter> PlayerCharacterClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AEnemyCharacter> EnemyCharacterClass;
	
};
