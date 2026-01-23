// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RAC_CPP_Character.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()

class MYRATCHET_API ARAC_CPP_Character : public ACharacter
{
	GENERATED_BODY()
	
private:
	
	UPROPERTY(EditAnywhere,Category="Appearance",meta= (AllowPrivateAccess = "true"))
	USkeletalMesh* CharacterMesh;
	
	UPROPERTY(EditAnywhere, Category = "Appearance",meta= (AllowPrivateAccess = "true"))
	TSubclassOf<UAnimInstance> AnimBlueprint;
	
	// SpringArm과 CameraComponent 생성
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta= (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta= (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	// inputAction, InputMappingContext (생성한 에셋을 변수에 담기)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input", meta= (AllowPrivateAccess = "true"))
	UInputMappingContext* CharacterMappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input", meta= (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input", meta= (AllowPrivateAccess = "true"))
	UInputAction* LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input", meta= (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input", meta= (AllowPrivateAccess = "true"))
	UInputAction* AimAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input", meta= (AllowPrivateAccess = "true"))
	UInputAction* DashAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input", meta= (AllowPrivateAccess = "true"))
	UInputAction* ShootAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input", meta= (AllowPrivateAccess = "true"))
	UInputAction* MeleeAction;
	
	//AimTurnSpeed
	UPROPERTY(EditAnywhere, Category="AimTurnning")
	float AimTurnSpeed = 12.f;
	
	//Dash
	// UPROPERTY(EditAnywhere, Category="Dash-AfterImage")
	// bool bUseAfterImage;
	//
	// UPROPERTY(EditAnywhere, Category="Dash-AfterImage")
	// float AfterImageInterval = 0.03f;
	//
	// UPROPERTY(EditAnywhere, Category="Dash-AfterImage")
	// float AfterImageLifeTime = 0.18f;
	//
	// UPROPERTY(EditAnywhere, Category="Dash-AfterImage")
	// UMaterialInterface* AfterImageMaterial = nullptr;
	
	// <<<   Dash   >>>
	UPROPERTY(EditAnywhere, Category="Dash")
	float DashDistance = 600.f;
	UPROPERTY(EditAnywhere, Category="Dash")
	float DashDuration = 0.12f;
	UPROPERTY(EditAnywhere, Category="Dash")
	UCurveFloat* DashEaseCurve = nullptr; //Ease 곡선
	UPROPERTY(EditAnywhere, Category="Dash")
	float DashCooldown = 0.35f;
	
	bool bIsDashing = false;
	bool bCanDash = true;
	bool bInvincible = false;
	
	FVector DashStart;
	FVector DashTarget;
	FVector DashDir;
	float DashElapsed = 0.f;
	
	FTimerHandle DashCooldownHandle;
	//FTimerHandle AfterImageTimerHandle;
	
	void StartDash();
	void TickDash(float DeltaTime);
	void EndDash(bool bInterrupted);
	
	//void SpawnAfterImage();
	
	
	// <<<  Glide   >>>
public:
	UPROPERTY(editAnywhere, BlueprintReadOnly, Category="Glide")
	bool bJumpHeld = false;
	
	UPROPERTY(editAnywhere, BlueprintReadOnly, Category="Glide")
	bool bGlideRequested = false;
	
	UPROPERTY(editAnywhere, BlueprintReadOnly, Category="Glide")
	bool bIsGliding = false;
	
private:
	FTimerHandle GlideHoldTimerHandle;

	float GlideHoldThreshold = 0.25f;
	
	
	float GlideGravityScale = 0.3f;
	float GlideFallSpeed = -300.0f;
	
	bool bWantsToGlide;
	float DefaultGravityScale = 1.5f;
	
	UPROPERTY(EditAnywhere,Category="Jump/Glide")
	float GlideThreshold = 0.3f;
	
	UPROPERTY(EditAnywhere, Category="Jump/Glide")
	float GlidingGravityScale = 0.2f;


public:
	// Sets default values for this character's properties
	ARAC_CPP_Character();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void OnConstruction(const FTransform& Transform) override;
	
	
	
protected:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Aim(const FInputActionValue& Value);
	void Shoot(const FInputActionValue& Value);
	void Dash(const FInputActionValue& Value);
	void Melee(const FInputActionValue& Value);
	
	void OnJumpStarted();
	
	void OnJumpCompleted();
	
	void TryStartGlideFromHold();
	
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

	void StartGlide();
	void StopGlide();
	
	virtual void Landed(const FHitResult& Hit) override;
	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stat")
	bool IsAiming = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stat")
	bool IsFireHold = false;
	
public:
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	
	float AimAlpha = 0.0f;
	float DefaultFOV = 90.0f;
	float AimingFOV = 60.0f;
	FVector AimingCameraLocation = FVector(0.0f,50.0f,0.0f);


};
