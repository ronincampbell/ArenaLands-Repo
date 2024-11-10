// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "HealthComponent.h"
#include "AGP/PlayerCharacterHUD.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void APlayerCharacter::UpdateHealthBar(float HealthPercent)
{
	if (PlayerHUD && IsLocallyControlled())
	{
		PlayerHUD->SetHealthBar(HealthPercent);
	}
}

void APlayerCharacter::UpdateAmmoUI(int32 RoundsRemaining, int32 MagazineSize)
{
	if (PlayerHUD && IsLocallyControlled())
	{
		PlayerHUD->SetAmmoText(RoundsRemaining, MagazineSize);
	}
}

void APlayerCharacter::UpdateModificationDetails(const FString& ModDetails)
{
	ServerUpdateModificationDetails(ModDetails);
}

void APlayerCharacter::UpdateGameOverVisibility(bool isGameOver)
{
	ServerUpdateGameOverVisibility(isGameOver);
}


void APlayerCharacter::ServerUpdateModificationDetails_Implementation(const FString& ModDetails)
{
	MulticastUpdateModificationDetails(ModDetails);
}

void APlayerCharacter::ServerUpdateGameOverVisibility_Implementation(bool isGameOver)
{
	MulticastUpdateGameOverVisibility(isGameOver);
}

void APlayerCharacter::MulticastUpdateModificationDetails_Implementation(const FString& ModDetails)
{
	if (PlayerHUD)
	{
		UE_LOG(LogTemp, Error, TEXT("Updating mods success"))
		PlayerHUD->SetModificationDetails(ModDetails);
		UpdateModsGraphical();
	}
}

void APlayerCharacter::MulticastUpdateGameOverVisibility_Implementation(bool isGameOver)
{
	if (PlayerHUD)
	{
		PlayerHUD->SetGameOverVisibility(isGameOver);
	}
}

void APlayerCharacter::DrawUI()
{
	if (IsLocallyControlled() && PlayerHUDClass)
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
		{
			PlayerHUD = CreateWidget<UPlayerCharacterHUD>(PlayerController, PlayerHUDClass);
			if (PlayerHUD)
			{
				PlayerHUD->AddToPlayerScreen();
			}
		}
	}
	UpdateHealthBar(1.0f);
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (const APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}

	DrawUI();
}

void APlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (PlayerHUD)
	{
		PlayerHUD->RemoveFromParent();
	}
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		Input->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
		Input->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		Input->BindAction(FireAction, ETriggerEvent::Triggered, this, &APlayerCharacter::FireWeapon);
		Input->BindAction(ReloadAction, ETriggerEvent::Started, this, &ABaseCharacter::Reload);
	}
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();
	const FVector ForwardVector = GetActorForwardVector();
	AddMovementInput(ForwardVector, MovementVector.X);
	const FVector RightVector = GetActorRightVector();
	AddMovementInput(RightVector, MovementVector.Y);
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookVector = Value.Get<FVector2D>() * LookSensitivity;
	AddControllerYawInput(LookVector.X);
	AddControllerPitchInput(LookVector.Y);
}

void APlayerCharacter::FireWeapon(const FInputActionValue& Value)
{
	FVector CameraPosition;
	FRotator CameraRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(CameraPosition, CameraRotation);
	const FVector CameraForward = UKismetMathLibrary::GetForwardVector(CameraRotation);
	if (BulletStartPosition)
	{
		Fire(BulletStartPosition->GetComponentLocation() + 10000.0f * CameraForward);
	}
}

