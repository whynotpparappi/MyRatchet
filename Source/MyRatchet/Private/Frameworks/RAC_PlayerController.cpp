// Fill out your copyright notice in the Description page of Project Settings.


#include "Frameworks/RAC_PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystemInterface.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "Characters/RAC_CPP_Character.h"

void ARAC_PlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(CharacterMappingContext,0);
	}
}

void ARAC_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		//jump
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ARAC_PlayerController::OnJumpStarted);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ARAC_PlayerController::OnJumpCompleted);

		//Move
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARAC_PlayerController::OnMove);

		//Look
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ARAC_PlayerController::OnLook);
		
		//Aim
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &ARAC_PlayerController::OnAim);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ARAC_PlayerController::OnAim);
		
		//Shooting
		if (ShootAction)
		{
			EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Started, this, &ARAC_PlayerController::OnShoot);
			EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Completed, this, &ARAC_PlayerController::OnShoot);
		}
		
		//Dash
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &ARAC_PlayerController::OnDash);
	
		//Melee
		EnhancedInputComponent->BindAction(MeleeAction,ETriggerEvent::Started, this, &ARAC_PlayerController::OnMelee);
	}
	else
	{
		UE_LOG(LogTemp,Warning,TEXT("Failed to Find an EnhancedInputComponent"));
	}
}

void ARAC_PlayerController::OnMove(const FInputActionValue& Value)
{
	if (ARAC_CPP_Character* C = Cast<ARAC_CPP_Character>(GetPawn()))
		C->Move(Value);
}

void ARAC_PlayerController::OnLook(const FInputActionValue& Value)
{
	if (ARAC_CPP_Character* C = Cast<ARAC_CPP_Character>(GetPawn()))
		C->Look(Value);
}

void ARAC_PlayerController::OnJumpStarted()
{
	if (ARAC_CPP_Character* C = Cast<ARAC_CPP_Character>(GetPawn()))
		C->JumpStarted();
}

void ARAC_PlayerController::OnJumpCompleted()
{
	if (ARAC_CPP_Character* C = Cast<ARAC_CPP_Character>(GetPawn()))
		C->JumpCompleted();
}

void ARAC_PlayerController::OnAim(const FInputActionValue& Value)
{
	if (ARAC_CPP_Character* C = Cast<ARAC_CPP_Character>(GetPawn()))
		C->Aim(Value);
}

void ARAC_PlayerController::OnShoot(const FInputActionValue& Value)
{
	if (ARAC_CPP_Character* C = Cast<ARAC_CPP_Character>(GetPawn()))
		C->Shoot(Value);
}

void ARAC_PlayerController::OnDash(const FInputActionValue& Value)
{
	if (ARAC_CPP_Character* C = Cast<ARAC_CPP_Character>(GetPawn()))
		C->Dash(Value);
}

void ARAC_PlayerController::OnMelee(const FInputActionValue& Value)
{
	if (ARAC_CPP_Character* C = Cast<ARAC_CPP_Character>(GetPawn()))
		C->Melee(Value);
}



