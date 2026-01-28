// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/RAC_AttributeSet.h"
#include "GameplayEffectExtension.h"

URAC_AttributeSet::URAC_AttributeSet()
{
	InitHealth(100.0f);
	InitMaxHealth(100.0f);
	InitBolts(0.0f);
	InitExperience(0.0f);
	
	InitCurrentAmmo(20.0f);
	InitMaxAmmo(20.0f);
	InitDamageMultiplier(1.0f);
	
	InitDashCooldown(0.0f);
}

void URAC_AttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	// 예: 체력이 MaxHealth를 넘지 못하게 하거나 0 아래로 내려가지 않게 클램핑
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
	}
}
