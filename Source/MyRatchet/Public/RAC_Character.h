// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "RAC_Character.generated.h"

UCLASS()
class MYRATCHET_API ARAC_Character : public ACharacter
{
	GENERATED_BODY()
	
private:
	//스프링암 + 카메라 컴포넌트 생성
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera" , meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	// InputAction, InputMappingContext (생성한 에셋을 변수에 담는다.)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* CharacterMappingContext;
	
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;
	
public:
	// Sets default values for this character's properties
	ARAC_Character();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

public:	
	
	//Getter
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }


	

};
