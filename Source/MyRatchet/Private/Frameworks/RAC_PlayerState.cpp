// Fill out your copyright notice in the Description page of Project Settings.


#include "Frameworks/RAC_PlayerState.h"
#include "AbilitySystemComponent.h"

ARAC_PlayerState::ARAC_PlayerState()
{
	// 1. ASC 생성
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	
	AbilitySystemComponent->SetIsReplicated(true);
	
	AttributeSet = CreateDefaultSubobject<URAC_AttributeSet>(TEXT("AtributeSet"));
}

UAbilitySystemComponent* ARAC_PlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
