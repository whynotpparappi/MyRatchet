// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "RAC_CPP_Character.generated.h"

struct FInputActionValue;
class USpringArmComponent;
class UCameraComponent;
class URAC_WeaponManager;
class URAC_WeaponData;

UCLASS()

class MYRATCHET_API ARAC_CPP_Character : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	// Sets default values for this character's properties
	ARAC_CPP_Character();
	
	
protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void PossessedBy(AController* NewController) override;
	
	virtual void OnRep_PlayerState() override;
	
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
	
	//AimTurnSpeed
	UPROPERTY(EditAnywhere, Category="AimTurnning")
	float AimTurnSpeed = 12.f;
	
	// <<<   Dash   >>>
	UPROPERTY(EditAnywhere, Category="Dash")
	float DashDistance = 600.f;
	UPROPERTY(EditAnywhere, Category="Dash")
	float DashDuration = 0.5f;
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

	// <<< Weapon >>>
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon", meta=(AllowPrivateAccess = "true"))
	URAC_WeaponManager* WeaponManager;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon", meta=(AllowPrivateAccess = "true"))
	TArray<URAC_WeaponData*> WeaponInventory;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon", meta=(AllowPrivateAccess = "true"))
	int32 CurrentWeaponIndex = -1;

	bool bWeaponInitialized = false;
	void TryInitializeWeapon();
	
	void StartDash();
	void TickDash(float DeltaTime);
	void EndDash(bool bInterrupted);
	
	//void SpawnAfterImage();
	
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Glide")
	bool bJumpHeld = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Glide")
	bool bGlideRequested = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Glide")
	bool bIsGliding = false;
	
private:
	// <<<  Glide   >>>
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


protected:
	
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

	void StartGlide();
	void StopGlide();
	
	virtual void Landed(const FHitResult& Hit) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stat")
	bool IsAiming = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stat")
	bool IsFireHold = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stat")
	bool IsTabHold = false;
	
public:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void JumpStarted();
	void JumpCompleted();
	void Aim(const FInputActionValue& Value);
	void Shoot(const FInputActionValue& Value);
    
	// Handle shooting locally (single-player)
	void HandleShoot(bool bPressed);
	void Dash(const FInputActionValue& Value);
	void Melee(const FInputActionValue& Value);
	void TryStartGlideFromHold();
	void Tab(const FInputActionValue& Value);
	
	virtual void OnConstruction(const FTransform& Transform) override;
	
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	
	float AimAlpha = 0.0f;
	float DefaultFOV = 90.0f;
	float AimingFOV = 60.0f;
	FVector AimingCameraLocation = FVector(0.0f,50.0f,0.0f);


};
