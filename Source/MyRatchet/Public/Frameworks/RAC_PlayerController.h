// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RAC_PlayerController.generated.h"

/**
 * 
 */
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class URAC_HUD_Widget;

UCLASS()
class MYRATCHET_API ARAC_PlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;
protected:
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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input", meta= (AllowPrivateAccess = "true"))
	UInputAction* TabAction;

	// HUD Widget 클래스 (블루프린트에서 설정)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI", meta= (AllowPrivateAccess = "true"))
	TSubclassOf<URAC_HUD_Widget> HUDWidgetClass;

	// 생성된 HUD Widget 참조
	UPROPERTY()
	URAC_HUD_Widget* HUDWidget;
	
private:
	void OnMove(const FInputActionValue& Value);
	void OnLook(const FInputActionValue& Value);
	void OnJumpStarted();
	void OnJumpCompleted();
	void OnAim(const FInputActionValue& Value);
	void OnShoot(const FInputActionValue& Value);
	void OnDash(const FInputActionValue& Value);
	void OnMelee(const FInputActionValue& Value);
	void OnTab(const FInputActionValue& Value);
	
	bool GetCrosshairRay(APlayerController* PC, FVector& OutStart, FVector& OutDir);
	bool GetAimTargetPoint(APlayerController* PC, AActor* IgnoreActor, float TraceDistance, FVector& OutTargetPoint);
};
