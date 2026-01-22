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
