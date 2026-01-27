// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/RAC_CPP_Character.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

#include "InputActionValue.h"
#include "Styling/StarshipCoreStyle.h"



// void ARAC_CPP_Character::SpawnAfterImage()
// {
// 	//
//}

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

	JumpMaxCount=2;

	GetCharacterMovement()->JumpZVelocity = 900.0f; //점프시 Z축 속도
	GetCharacterMovement()->GravityScale = 1.5f;
	GetCharacterMovement()->AirControl = 1.0f; //공중상태에서 떠있는 속도를 제어
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
	
	DefaultGravityScale = GetCharacterMovement()->GravityScale;
}

// Called every frame
void ARAC_CPP_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bIsDashing)
	{
		TickDash(DeltaTime);
	}
	
	if (!bIsGliding
		&& bGlideRequested
		&& bJumpHeld
		&& GetCharacterMovement()->IsFalling()
		&& GetVelocity().Z < 0.f)
	{
		StartGlide();
	}
	
	//에임 도는거 방지
	const bool bLockToController = (IsAiming || IsFireHold);
	if (bLockToController && Controller)
	{
		const float TargetYaw = Controller->GetControlRotation().Yaw;
		const FRotator Current = GetActorRotation();
		const FRotator Target(0.f,TargetYaw,0.f);
		
		const FRotator NewRot = FMath::RInterpTo(Current,Target,DeltaTime,AimTurnSpeed);
		SetActorRotation(NewRot);
	}
	
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
	Super::SetupPlayerInputComponent(PlayerInputComponent);
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

void ARAC_CPP_Character::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	bJumpHeld = false;
	GetWorldTimerManager().ClearTimer(GlideHoldTimerHandle);
	StopGlide();
}

void ARAC_CPP_Character::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
	const bool bNowFalling = GetCharacterMovement()->IsFalling();
	
	// UE_LOG(LogTemp, Warning, TEXT("ModeChanged NowFalling=%d Requested=%d Held=%d VelZ=%.2f"),
	// 	GetCharacterMovement()->IsFalling(),
	// 	bGlideRequested,
	// 	bJumpHeld,
	// 	GetVelocity().Z);
	
	if(bNowFalling && bGlideRequested && bJumpHeld && !bIsGliding)
	{
		// if (GetVelocity().Z <= 0.f)
		// {
		// 	StartGlide();
		// }
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
	IsFireHold = Value.Get<bool>();
	if (Controller)
	{
		bUseControllerRotationYaw = IsFireHold;
		if (GetCharacterMovement())
		{
			GetCharacterMovement()->bOrientRotationToMovement = !IsAiming;
		}
	}
}

void ARAC_CPP_Character::Dash(const FInputActionValue& Value)
{
	StartDash();
}


void ARAC_CPP_Character::Melee(const FInputActionValue& Value)
{
}

void ARAC_CPP_Character::JumpStarted()
{
	bJumpHeld = true;
	bGlideRequested = false;
	
	GetWorldTimerManager().SetTimer(
		GlideHoldTimerHandle,
		this,
		&ThisClass::TryStartGlideFromHold,
		GlideHoldThreshold,
		false);
	
	Jump();
}

void ARAC_CPP_Character::JumpCompleted()
{
	UE_LOG(LogTemp,Warning,TEXT("Jump completed"));
	bJumpHeld = false;
	GetWorldTimerManager().ClearTimer(GlideHoldTimerHandle);
	
	bGlideRequested = false;
	
	if (bIsGliding)
		StopGlide();
}

void ARAC_CPP_Character::TryStartGlideFromHold()
{
	// UE_LOG(LogTemp, Warning, TEXT("HoldReached bJumpHeld=%d Falling=%d VelZ=%.2f"),
	// 	bJumpHeld,
	// 	GetCharacterMovement()->IsFalling(),
	// 	GetVelocity().Z);
	if (!bJumpHeld) return;

	bGlideRequested = true;
}

void ARAC_CPP_Character::StartGlide()
{
	UE_LOG(LogTemp,Warning,TEXT("GlideStart"));
	if (bIsGliding) return;
	if (!GetCharacterMovement()->IsFalling()) return;
	
	bIsGliding=true;
	
    UCharacterMovementComponent* Move = GetCharacterMovement();
	
	Move->GravityScale = GlideGravityScale;
}

void ARAC_CPP_Character::StopGlide()
{
	if (!bIsGliding) return;
	bIsGliding=false;
	
    UCharacterMovementComponent* Move = GetCharacterMovement();
	Move->GravityScale = DefaultGravityScale;
}




void ARAC_CPP_Character::StartDash()
{
	UE_LOG(LogTemp, Warning, TEXT("LastInput=%s  ActorForward=%s  ControlYaw=%.2f"),
	*GetLastMovementInputVector().ToString(),
	*GetActorForwardVector().ToString(),
	Controller ? Controller->GetControlRotation().Yaw : -999.f
);
	
	if (!bCanDash || bIsDashing)
		return;
	
	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	if (!MoveComp)
		return;
	
	// if (bUseAfterImage && AfterImageMaterial)
	// {
	// 	GetWorldTimerManager().SetTimer(
	// 	AfterImageTimerHandle,
	// 	this,
	// 	&ARAC_CPP_Character::SpawnAfterImage,
	// 	AfterImageInterval,
	// 	true
	// 	);
	// }
	
	// 1. 방향 결정 : "현재 이동 입력"이 있으면 그쪽, 아니면 forward
	FVector InputDir = GetLastMovementInputVector();
	DashDir = InputDir.IsNearlyZero() ? GetActorForwardVector() : InputDir.GetSafeNormal2D();
	
	DashDir.Z = 0.f;
	DashDir = DashDir.GetSafeNormal();
	
	// 2. 시작 / 목표
	DashStart = GetActorLocation();
	DashTarget = DashStart + DashDir * DashDistance;
	
	UE_LOG(LogTemp, Warning, TEXT("DashStart=%s DashDir=%s DashTarget=%s"),
	*DashStart.ToString(),
	*DashDir.ToString(),
	*DashTarget.ToString()
);
	
	// 3. 상태 전환
	bIsDashing = true;
	bCanDash = false;
	bInvincible = true;
	DashElapsed = 0.f;
	
	// 4. 대쉬 중 캐릭터 무브먼트 영향 최소화
	MoveComp->StopMovementImmediately();
	MoveComp->SetMovementMode(EMovementMode::MOVE_Flying);
	MoveComp->Velocity = FVector::ZeroVector;
	
	
	GetWorldTimerManager().ClearTimer(DashCooldownHandle);
}

void ARAC_CPP_Character::TickDash(float DeltaTime)
{
	DashElapsed += DeltaTime;
	
	float Alpha = FMath::Clamp(DashElapsed/DashDuration, 0.f, 1.f);
	
	//Ease 적용
	float T = Alpha;
	if (DashEaseCurve)
	{
		T = DashEaseCurve->GetFloatValue(Alpha);
	}
	else
	{
		T = FMath::InterpEaseOut(0.f,1.f,Alpha,3.f);
	}
	
	FVector NewPos = FMath::Lerp(DashStart,DashTarget,T);
	
	FHitResult Hit;
	SetActorLocation(NewPos,true,&Hit,ETeleportType::None);
	
	if (Hit.bBlockingHit)
	{
		EndDash(true);
		return;
	}
	if (Alpha >= 1.f)
	{
		EndDash(false);
	}
}

void ARAC_CPP_Character::EndDash(bool bInterrupted)
{
	if (!bIsDashing) return;

	bIsDashing = false;
	bInvincible = false;
	//GetWorldTimerManager().ClearTimer(AfterImageTimerHandle);

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->SetMovementMode(EMovementMode::MOVE_Walking);

		// 팬텀대쉬 느낌: 끝에서 "툭" 멈추기
		MoveComp->Velocity = FVector::ZeroVector;

		// (옵션) 살짝 관성 남기고 싶으면:
		// MoveComp->Velocity = DashDir * 200.f;
	}

	// 쿨다운 끝나면 다시 대쉬 가능
	GetWorldTimerManager().SetTimer(
		DashCooldownHandle,
		[this]()
		{
			bCanDash = true;
		},
		DashCooldown,
		false
	);
}

