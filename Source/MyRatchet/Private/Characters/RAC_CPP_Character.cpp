// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/RAC_CPP_Character.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

#include "InputActionValue.h"


// Sets default values
ARAC_CPP_Character::ARAC_CPP_Character()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
    GetCapsuleComponent()->InitCapsuleSize(42.0f, 95.0f);
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	//캐릭터 무브먼트 설정
	GetCharacterMovement()->bOrientRotationToMovement = true; //입력된 방향으로 캐릭터가 움직인다.
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); //Z측기준으로 회전속도 540부여

	GetCharacterMovement()->JumpZVelocity = 900.0f; //점프시 Z축 속도
	GetCharacterMovement()->GravityScale = 1.5f;
	GetCharacterMovement()->AirControl = 1.0f; //공중상태에서 떠있는 속도를 제어
	GetCharacterMovement()->MaxJumpApexAttemptsPerSimulation = 2;
	GetCharacterMovement()->MaxWalkSpeed = 600.0f; //최대걷는속도
	GetCharacterMovement()->BrakingDecelerationWalking = 2048.0f; //제동속도
	
	//스프링암 세팅
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetRelativeLocation(FVector(0.0f, 0.0f, 110.0f));
	CameraBoom->TargetArmLength = 500.0f;
	CameraBoom->bUsePawnControlRotation = true; //컨트롤러에 따라 스프링암이 회전을 한다.
	
	//카메라 세팅
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; //컨트롤러에 따라 카메라 붐을 회전시킨다.

}

// Called when the game starts or when spawned
void ARAC_CPP_Character::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARAC_CPP_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	
	UE_LOG(LogTemp, Warning, TEXT("IsAiming: %s, OrientRotation: %s, UseControllerYaw: %s"), 
	IsAiming ? TEXT("True") : TEXT("False"), 
	GetCharacterMovement()->bOrientRotationToMovement ? TEXT("True") : TEXT("False"),
	bUseControllerRotationYaw ? TEXT("True") : TEXT("False"));
	
	//1. Aim Alpha 계산
	float TargetAlpha = IsAiming ? 1.0f : 0.0f;
	AimAlpha = FMath::FInterpTo(AimAlpha, TargetAlpha, DeltaTime, 15.0f);
	
	//2. FOV 적용 (Lerp + SetFieldOfView 매칭)
	float NewFOV = FMath::Lerp(DefaultFOV, AimingFOV, AimAlpha);
	FollowCamera->SetFieldOfView(NewFOV);
	
	//3. 카메라 위치 적용
	FVector NewLocation = FMath::Lerp(FVector::ZeroVector, AimingCameraLocation, AimAlpha);
	FollowCamera->SetRelativeLocation(NewLocation);

}

// Called to bind functionality to input
void ARAC_CPP_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(CharacterMappingContext,0);
		}
	}
	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//jump
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Move
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARAC_CPP_Character::Move);

		//Look
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ARAC_CPP_Character::Look);
		
		//Aim
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &ARAC_CPP_Character::Aim);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ARAC_CPP_Character::Aim);
		
		//Shooting
		if (ShootAction)
		{
			EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &ARAC_CPP_Character::Shoot);
			EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ARAC_CPP_Character::Shoot);
		}
	}
	else
	{
		UE_LOG(LogTemp,Warning,TEXT("Failed to Find an EnhancedInputComponent"));
	}
}

void ARAC_CPP_Character::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (CharacterMesh)
	{
		GetMesh()->SetSkeletalMesh(CharacterMesh);
	}
	if (AnimBlueprint)
	{
		GetMesh()->SetAnimInstanceClass(AnimBlueprint);
	}
}

void ARAC_CPP_Character::Move(const FInputActionValue& Value)
{
	//Valuer값을 2D Vector로 받기 (X와Y)
	FVector2D MovementVector = Value.Get<FVector2D>();
	
	if (Controller)
	{
		//정면을 구하기 위한 Yaw축 회전 추출
		FRotator YawRotation(0,  Controller->GetControlRotation().Yaw, 0);

		//정면 방향 구하기
		FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		//우측면 구하기
		FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		//움직임 추가
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ARAC_CPP_Character::Look(const FInputActionValue& Value)
{
	//3) Look InputAction의 기능구현(Multiple Function)
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		//컨트롤러에 X(Yaw)축, Y(Pitch)축 입력 추가
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ARAC_CPP_Character::Aim(const FInputActionValue& Value)
{
	IsAiming = Value.Get<bool>();
	if (Controller)
	{
		bUseControllerRotationYaw = IsAiming;
		
		if (GetCharacterMovement())
		{
			GetCharacterMovement()->bOrientRotationToMovement = !IsAiming;
		}
	}
}

void ARAC_CPP_Character::Shoot(const FInputActionValue& Value)
{
	bool FireHold = Value.Get<bool>();
	if (Controller != nullptr)
	{
		IsFireHold = FireHold;
	}
}

void ARAC_CPP_Character::Dash(const FInputActionValue& Value)
{
	
}





