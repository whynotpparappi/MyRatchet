// Fill out your copyright notice in the Description page of Project Settings.


#include "Frameworks/RAC_PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystemInterface.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "Characters/RAC_CPP_Character.h"
#include "Widgets/RAC_HUD_Widget.h"

void ARAC_PlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(CharacterMappingContext,0);
	}

	// HUD Widget 생성
	if (HUDWidgetClass && IsLocalController())
	{
		HUDWidget = CreateWidget<URAC_HUD_Widget>(this, HUDWidgetClass);
		if (HUDWidget)
		{
			HUDWidget->AddToViewport();
		}
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
	
		//Tab
		EnhancedInputComponent->BindAction(TabAction,ETriggerEvent::Triggered, this, &ARAC_PlayerController::OnTab);
		EnhancedInputComponent->BindAction(TabAction, ETriggerEvent::Completed, this, &ARAC_PlayerController::OnTab);
		
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

void ARAC_PlayerController::OnTab(const FInputActionValue& Value)
{
	if (ARAC_CPP_Character* C = Cast<ARAC_CPP_Character>(GetPawn()))
	{
		C->Tab(Value);
	}
}

bool ARAC_PlayerController::GetCrosshairRay(APlayerController* PC, FVector& OutStart, FVector& OutDir)
{
	if (!PC) return false;
	
	int32 SizeX, SizeY;
	PC->GetViewportSize(SizeX, SizeY);
	
	FVector WorldLoc, WorldDir;
	const float ScreenX = SizeX * 0.5f;
	const float ScreenY = SizeY * 0.5f;
	
	if (PC->DeprojectScreenPositionToWorld(ScreenX, ScreenY, WorldLoc, WorldDir))
		return false;
	
	OutStart = WorldLoc;
	OutDir = WorldDir.GetSafeNormal();
	return true;
	
}

bool ARAC_PlayerController::GetAimTargetPoint(
	APlayerController* PC,
	AActor* IgnoreActor,
	float TraceDistance,
	FVector& OutTargetPoint)
{
	FVector Start, Dir;
	if (!GetCrosshairRay(PC, Start, Dir))
		return false;
	
	FVector End = Start + Dir * TraceDistance;
	
	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(AimTrace), true, IgnoreActor);
	Params.AddIgnoredActor(IgnoreActor);
	
	bool bHit = PC->GetWorld()->LineTraceSingleByChannel(
		Hit,Start,End,ECC_Visibility, Params
		);
	
	OutTargetPoint = bHit ? Hit.ImpactPoint : End;
	return true;
}



