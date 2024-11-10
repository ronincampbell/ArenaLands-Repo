// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacterHUD.h"

#include "MultiplayerGameMode.h"
#include "Components/CanvasPanel.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UPlayerCharacterHUD::SetHealthBar(float HealthPercent)
{
	if (HealthBar)
	{
		HealthBar->SetPercent(HealthPercent);
	}
}

void UPlayerCharacterHUD::SetAmmoText(int32 RoundsRemaining, int32 MagazineSize)
{
	if (AmmoText)
	{
		AmmoText->SetText(FText::FromString(FString::FromInt(RoundsRemaining) + " / " + FString::FromInt(MagazineSize)));
	}
}

void UPlayerCharacterHUD::SetModificationDetails(const FString& WepMods)
{
	if(ModDetails)
	{
		ModDetails->SetText(FText::FromString(WepMods));
	}
}

void UPlayerCharacterHUD::SetGameOverVisibility(bool IsGameOver)
{
	if(GameOverPanel)
	{
		if(IsGameOver)
		{
			GameOverPanel->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			GameOverPanel->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UPlayerCharacterHUD::OnRestart()
{
	if (AMultiplayerGameMode* GameMode = Cast<AMultiplayerGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->RespawnAllPlayers();
	}
}
