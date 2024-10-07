// Fill out your copyright notice in the Description page of Project Settings.


#include "TreasurePickup.h"

#include "AGP/Characters/BaseCharacter.h"

void ATreasurePickup::OnPickupOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& HitInfo)
{
	if (ABaseCharacter* Player = Cast<ABaseCharacter>(OtherActor))
	{
		Destroy();
	}
}
