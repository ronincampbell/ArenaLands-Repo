// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include "EngineUtils.h"
#include "HealthComponent.h"
#include "PlayerCharacter.h"
#include "AGP/Pathfinding/PathfindingSubsystem.h"
#include "Perception/PawnSensingComponent.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>("Pawn Sensing Component");

	//CoverCheckPosition = CreateDefaultSubobject<USceneComponent>("Cover check");
	//CoverCheckPosition->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	PathfindingSubsystem = GetWorld()->GetSubsystem<UPathfindingSubsystem>();
	if (PathfindingSubsystem)
	{
		//CurrentPath = PathfindingSubsystem->GetRandomPath(GetActorLocation());
	} else
	{
		UE_LOG(LogTemp, Error, TEXT("Unable to find the PathfindingSubsystem"))
	}
	if (PawnSensingComponent)
	{
		PawnSensingComponent->OnSeePawn.AddDynamic(this, &AEnemyCharacter::OnSensedPawn);
	}

	int HighestExistingID = -1;
	//Making sure squads are fully interconnected
	for(AEnemyCharacter* SquadMate : SquadMates)
	{
		if(!SquadMate->SquadMates.Contains(this))
		{
			SquadMate->AddSquadMate(this);
		}
		if(SquadMate->SquadID > HighestExistingID)
		{
			HighestExistingID = SquadMate->SquadID;
		}
	}

	SquadID = HighestExistingID + 1;
	
	if(HasTreasure())
	{
		FVector GuardDirection = FVector::ForwardVector.RotateAngleAxis(360/(SquadMates.Num()+1) * SquadID, FVector::UpVector);
		GuardLocation = PathfindingSubsystem->FindNearestNodePos(Treasure->GetActorLocation()+GuardDirection*GuardRadius);
	}
	else
	{
		GuardLocation = PathfindingSubsystem->FindNearestNodePos(GetActorLocation());
	}
	//CurrentPath.Add(GuardLocation);
}

void AEnemyCharacter::MoveAlongPath()
{
	// Execute the path. Should be called each tick.

	// If the path is empty do nothing.
	if (CurrentPath.IsEmpty()) return;
	
	// 1. Move towards the current stage of the path.
	//		a. Calculate the direction from the current position to the target of the current stage of the path.
	FVector MovementDirection = CurrentPath[CurrentPath.Num()-1] - GetActorLocation();
	MovementDirection.Normalize();
	//		b. Apply movement in that direction.
	AddMovementInput(MovementDirection);
	// 2. Check if it is close to the current stage of the path then pop it off.
	if (FVector::Distance(GetActorLocation(), CurrentPath[CurrentPath.Num() - 1]) < PathfindingError)
	{
		CurrentPath.Pop();
	}
}

void AEnemyCharacter::TickGuard()
{
	//Potentially could distribute look direction with other guarding squad mates
	if(CurrentPath.IsEmpty())
	{
		CurrentPath = PathfindingSubsystem->GetPath(GetActorLocation(), GuardLocation);

		if(HasTreasure())
		{
			FVector DirectionFromTreasure = GetActorLocation() - Treasure->GetActorLocation();
			FVector FlatDirection = DirectionFromTreasure.GetSafeNormal2D();

			FRotator FlatRotation = FlatDirection.Rotation();
			SetActorRotation(FlatRotation);
		}
	}

	MoveAlongPath();
}

void AEnemyCharacter::TickPatrol()
{
	//Potentially could distribute patrol paths with other patrolling squad mates

	if(CurrentPath.IsEmpty())
	{
		if(!HasTreasure())
		{
			EnterIdle();
			return;
		}
		const FVector RandOffset = FMath::VRand().GetSafeNormal2D()*TerritoryRadius;
		const FVector PatrolLocation = Treasure->GetActorLocation() + RandOffset;
		CurrentPath = PathfindingSubsystem->GetPath(GetActorLocation(), PatrolLocation);
	}

	MoveAlongPath();
}

void AEnemyCharacter::TickWander()
{
	if(CurrentPath.IsEmpty())
	{
		CurrentPath = PathfindingSubsystem->GetRandomPath(GetActorLocation());
	}

	MoveAlongPath();
}

void AEnemyCharacter::TickInvestigate(float DeltaTime)
{
	if(CurrentPath.IsEmpty())
	{
		CurrentPath = PathfindingSubsystem->GetPath(GetActorLocation(), LastSeenPlayerLocation);
	}

	if(FVector::Distance(GetActorLocation(), LastSeenPlayerLocation) <= InvestigateError)
	{
		InvestigateTimer += DeltaTime;
		if(InvestigateTimer >= InvestigateTimeout)
		{
			InvestigateTimer = 0.0f;
			EnterIdle();
		}
	}
	
	MoveAlongPath();
}

void AEnemyCharacter::TickRetreat()
{
	if(CurrentPath.IsEmpty())
	{
		FVector RetreatDirection = GetActorLocation() - GetCurrOrLastPlayerPos();
		
		CurrentPath.Add(PathfindingSubsystem->GetPosInDirection(GetActorLocation(), RetreatDirection));
	}

	MoveAlongPath();
}

void AEnemyCharacter::TickHold(float DeltaTime)
{
	UpdateCover();
	if(CurrentPath.IsEmpty())
	{
		if(!InCover)
		{
			if(HasTreasure())
			{
				CurrentPath = PathfindingSubsystem->GetPath(GetActorLocation(), FindNearestCoverLocation(Treasure->GetActorLocation()));
			}
			else
			{
				CurrentPath = PathfindingSubsystem->GetPath(GetActorLocation(), FindNearestCoverLocation(GetActorLocation()));
			}
		}
	}

	MoveAlongPath();
	
	if(!InCover)
	{
		UnCrouch();
		UE_LOG(LogTemp, Display, TEXT("Early Return"))
		return;
	}

	CoverTimer += DeltaTime;
	if(bIsCrouched)
	{
		if(CoverTimer > MinimumHideDuration && (!HasWeapon() || !WeaponComponent->IsMagazineEmpty()))
		{
			UnCrouch();
			CoverTimer = 0.0f;
		}
	}
	else
	{
		Fire(GetCurrOrLastPlayerPos());
		
		if(CoverTimer > MaximumPopupDuration || (HasWeapon() && WeaponComponent->IsMagazineEmpty()))
		{
			Crouch();
			CoverTimer = 0.0f;
		}
	}

}

void AEnemyCharacter::TickPush()
{
	if(CurrentPath.IsEmpty())
	{
		CurrentPath = PathfindingSubsystem->GetPath(GetActorLocation(), GetCurrOrLastPlayerPos());
	}

	MoveAlongPath();
	Fire(GetCurrOrLastPlayerPos());
}

void AEnemyCharacter::TickScatter(float DeltaTime)
{
	if(CurrentPath.IsEmpty())
	{
		FVector ScatterDirection = GetActorLocation() - LastExplosiveLocation;
	
		CurrentPath.Add(PathfindingSubsystem->GetPosInDirection(GetActorLocation(), ScatterDirection));
	}

	ScatterTimer -= DeltaTime;
	if(ScatterTimer <= 0.0f)
	{
		ScatterTimer = 0.0f;
		if(DetectedPlayer)
		{
			EnterCombat();
		}
		else
		{
			EnterIdle();
		}
	}

	MoveAlongPath();
}

void AEnemyCharacter::OnSensedPawn(APawn* SensedActor)
{
	if (APlayerCharacter* Player = Cast<APlayerCharacter>(SensedActor))
	{
		if(!Player->IsSpectator()){
			SensedCharacter = Player;
		}
	}
}

void AEnemyCharacter::OnHearExplosion(const FVector& ExplosionLocation)
{
	LastExplosiveLocation = ExplosionLocation;
	ScatterTimer = ScatterTimeout;
	CurrentPath.Empty();
	CurrentState = EEnemyState::Scatter;
}

void AEnemyCharacter::UpdateSight()
{
	if (!SensedCharacter) return;
	if (PawnSensingComponent)
	{
		if (!PawnSensingComponent->HasLineOfSightTo(SensedCharacter))
		{
			LastSeenPlayerLocation = PathfindingSubsystem->FindNearestNodePos(SensedCharacter->GetActorLocation());
			LastSeenPlayerHealth = SensedCharacter->HealthComponent->GetCurrentHealthPercentage();
			SensedCharacter = nullptr;
			//UE_LOG(LogTemp, Display, TEXT("Lost Player"))
		}
	}
}


void AEnemyCharacter::UpdateMorale()
{
	float NewMorale = 0.0f;
	NewMorale += HealthComponent->GetCurrentHealthPercentage()*HealthMoraleContribution;
	NewMorale += SquadMates.Num()*SquadMateMoraleContribution;
	if(SensedCharacter && DetectedPlayer)
	{
		NewMorale -= SensedCharacter->HealthComponent->GetCurrentHealthPercentage()*PlayerHealthMoraleContribution;
	}
	else
	{
		NewMorale -= LastSeenPlayerHealth*PlayerHealthMoraleContribution;
	}

	CurrentMorale = NewMorale;
	if(IsInCombat()){
		EnterCombat();
	}
}

void AEnemyCharacter::UpdateCover()
{
	FVector PlayerPosition = GetCurrOrLastPlayerPos() + PlayerHeadOffset;
	
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	for(AEnemyCharacter* SquadMate : SquadMates)
	{
		QueryParams.AddIgnoredActor(SquadMate);
	}
	if (GetWorld()->LineTraceSingleByChannel(HitResult, GetActorLocation() + FootOffset + CoverCheckOffset, PlayerPosition, ECC_WorldStatic, QueryParams))
	{
		//If the line trace didn't hit the player
		//ie. Cover intercepted the line
		if (!Cast<APlayerCharacter>(HitResult.GetActor()))
		{
			InCover = true;
			return;
		}
	}

	InCover = false;
}

bool AEnemyCharacter::HasTreasure()
{
	return IsValid(Treasure);
}

void AEnemyCharacter::EnterCombat()
{
	EEnemyState DesiredState {};
	if(CurrentMorale < MoraleRetreatCutoff)
	{
		DesiredState = EEnemyState::Retreat;
	}
	else if(CurrentMorale < MoralePushCutoff)
	{
		DesiredState = EEnemyState::Hold;
	}
	else
	{
		DesiredState = EEnemyState::Push;
	}
	
	if(DesiredState != CurrentState)
	{
		CurrentPath.Empty();
		CurrentState = DesiredState;
	}
}

void AEnemyCharacter::EnterIdle()
{
	EEnemyState DesiredState {};
	if(!HasTreasure())
	{
		DesiredState = EEnemyState::Wander;
	}
	else if(PatrolDuty)
	{
		DesiredState = EEnemyState::Patrol;
	}
	else
	{
		DesiredState = EEnemyState::Guard;
	}
	
	if(DesiredState != CurrentState)
	{
		CurrentPath.Empty();
		CurrentState = DesiredState;
	}
}

bool AEnemyCharacter::IsInCombat() const
{
	return CurrentState == EEnemyState::Retreat || CurrentState == EEnemyState::Hold || CurrentState == EEnemyState::Push;
}

void AEnemyCharacter::ReceiveCallout(APlayerCharacter* SensedPlayer)
{
	//UE_LOG(LogTemp, Display, TEXT("Received callout"))
	SensedCharacter = SensedPlayer;
	FVector DirectionToPlayer = SensedPlayer->GetActorLocation() - GetActorLocation();
	FVector FlatDirection = DirectionToPlayer.GetSafeNormal2D();

	FRotator FlatRotation = FlatDirection.Rotation();
	SetActorRotation(FlatRotation);
	
	DetectedPlayer = true;
	DetectionTimer = ReturnToIdleDelay;
	EnterCombat();
}

void AEnemyCharacter::SendCallouts()
{
	for(AEnemyCharacter* SquadMate : SquadMates)
	{
		SquadMate->ReceiveCallout(SensedCharacter);
	}
}

FVector AEnemyCharacter::FindNearestCoverLocation(const FVector& StartLocation) const
{
	FVector NearestPos{};
	bool FoundCover = false;
	for(FVector NodePos : PathfindingSubsystem->GetWaypointPositions())
	{
		float NodeDistance = FVector::Distance(StartLocation, NodePos);
		if(NodeDistance > CoverCheckDistance)
		{
			continue;
		}

		FVector CheckPosition = NodePos + CoverCheckOffset;
		FVector PlayerPosition = GetCurrOrLastPlayerPos()+PlayerHeadOffset;

		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		for(AEnemyCharacter* SquadMate : SquadMates)
		{
			QueryParams.AddIgnoredActor(SquadMate);
		}
		if (GetWorld()->LineTraceSingleByChannel(HitResult, CheckPosition, PlayerPosition, ECC_WorldStatic, QueryParams))
		{
			//If the line trace didn't hit the player
			//ie. Cover intercepted the line
			if (!Cast<APlayerCharacter>(HitResult.GetActor()))
			{
				if(!FoundCover)
				{
					NearestPos = NodePos;
					FoundCover = true;
				}
				else
				{
					if(NodeDistance < FVector::Distance(StartLocation, NearestPos))
					{
						NearestPos = NodePos;
					}
				}
			}
		}
	}

	if(!FoundCover)
	{
		return StartLocation;		
	}
	else
	{
		return NearestPos;
	}
}

void AEnemyCharacter::AddSquadMate(AEnemyCharacter* NewSquadMate)
{
	SquadMates.Add(NewSquadMate);

	//Add squad mates of new squad mate
	for(AEnemyCharacter* SquadMate : NewSquadMate->SquadMates)
	{
		if(SquadMate == this)
		{
			continue;
		}
		if(!SquadMates.Contains(SquadMate))
		{
			AddSquadMate(SquadMate);
		}
	}
}

// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Handle destroying self and removing self from squad
	if(HealthComponent->IsDead())
	{
		for(AEnemyCharacter* SquadMate : SquadMates)
		{
			SquadMate->SquadMates.Remove(this);
		}

		Destroy();
		return;
	}

	if(HasWeapon())
	{
		if(WeaponComponent->IsMagazineEmpty())
		{
			Reload();
		}
	}
	
	UpdateSight();
	UpdateMorale();

	if(ScatterTimer > 0)
	{
		//This space intentionally left blank
		//This condition just prevents the detection code from running and interrupting scattering
	}
	//If player is spotted while idle
	else if(SensedCharacter && !DetectedPlayer)
	{
		DetectionTimer += DeltaTime;

		//After seeing long enough to surpass detection delay
		if(DetectionTimer >= DetectionDelay)
		{
			//Set timer to idle delay, so the enemy takes longer to return to idle after losing sight line
			DetectionTimer = ReturnToIdleDelay;
			DetectedPlayer = true;
			//Inform squad of player position
			if(HasTreasure())
			{
				SendCallouts();
			}
			EnterCombat();
		}
	}
	//If sight line is lost
	else if(!SensedCharacter)
	{
		if(DetectionTimer > 0)
		{
			DetectionTimer -= DeltaTime;

			if(DetectionTimer <= 0)
			{
				DetectionTimer = 0;
				//Move to idle if lost sight line during combat
				if(DetectedPlayer)
				{
					EnterIdle();
				}
				//Investigate if sight line lost without full detection
				else
				{
					CurrentPath.Empty();
					CurrentState = EEnemyState::Investigate;
				}
				DetectedPlayer = false;
			}
		}
	}

	if(bIsCrouched && CurrentState != EEnemyState::Hold)
	{
		UnCrouch();
		CoverTimer = 0.0f;
	}
	
	switch(CurrentState){
	case EEnemyState::Guard:
		TickGuard();
		if(bPrintState) UE_LOG(LogTemp, Display, TEXT("State: Guard"))
		break;
	case EEnemyState::Patrol:
		TickPatrol();
		if(bPrintState) UE_LOG(LogTemp, Display, TEXT("State: Patrol"))
		break;
	case EEnemyState::Investigate:
		TickInvestigate(DeltaTime);
		if(bPrintState) UE_LOG(LogTemp, Display, TEXT("State: Investigate"))
		break;
	case EEnemyState::Wander:
		TickWander();
		if(bPrintState) UE_LOG(LogTemp, Display, TEXT("State: Wander"))
		break;
	case EEnemyState::Hold:
		TickHold(DeltaTime);
		if(bPrintState) UE_LOG(LogTemp, Display, TEXT("State: Hold"))
		break;
	case EEnemyState::Push:
		TickPush();
		if(bPrintState) UE_LOG(LogTemp, Display, TEXT("State: Push"))
		break;
	case EEnemyState::Retreat:
		TickRetreat();
		if(bPrintState) UE_LOG(LogTemp, Display, TEXT("State: Retreat"))
		break;
	case EEnemyState::Scatter:
		TickScatter(DeltaTime);
		if(bPrintState) UE_LOG(LogTemp, Display, TEXT("State: Scatter"))
		break;
	}

	/*UE_LOG(LogTemp, Display, TEXT("Morale: %f"), CurrentMorale)
	UE_LOG(LogTemp, Display, TEXT("Cover: %hs"), InCover ? "True" : "False")
	UE_LOG(LogTemp, Display, TEXT("Sensed: %hs"), SensedCharacter ? "True" : "False")
	UE_LOG(LogTemp, Display, TEXT("Detected: %hs"), DetectedPlayer ? "True" : "False")*/
}

// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

APlayerCharacter* AEnemyCharacter::FindPlayer() const
{
	APlayerCharacter* Player = nullptr;
	for (TActorIterator<APlayerCharacter> It(GetWorld()); It; ++It)
	{
		Player = *It;
		break;
	}
	if (!Player)
	{
		UE_LOG(LogTemp, Error, TEXT("Unable to find the Player Character in the world."))
	}
	return Player;
}

FVector AEnemyCharacter::GetCurrOrLastPlayerPos() const
{
	if(DetectedPlayer && SensedCharacter)
	{
		return SensedCharacter->GetActorLocation();
	}
	
	return LastSeenPlayerLocation;
}