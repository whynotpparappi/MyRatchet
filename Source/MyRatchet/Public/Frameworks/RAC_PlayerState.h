// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "Characters/RAC_AttributeSet.h"
#include "RAC_PlayerState.generated.h"

/**
 * 
 */
UCLASS()
class MYRATCHET_API ARAC_PlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	ARAC_PlayerState();
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	// 이 장부를 쓰겠다고 선언하는 부분
	UPROPERTY()
	class URAC_AttributeSet* AttributeSet;
	
private:
	UPROPERTY(VisibleAnywhere,Category="Abilities")
	class UAbilitySystemComponent* AbilitySystemComponent;
	
	
};
