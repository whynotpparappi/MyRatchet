// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/RAC_AttributeSet.h"
#include "GameplayEffectExtension.h"

URAC_AttributeSet::URAC_AttributeSet()
{
	// 1.
	InitHealth(100.0f);
	InitMaxHealth(100.0f);
	InitExperience(0.0f);
	InitLevel(1);
	InitDamageMultiplier(1.f);
	// 2.
	InitBolts(0);
	InitRaritanium(0);
	// 3.
	
	InitAmmo(0);
	InitMaxAmmo(150);
	
	InitDashCooldown(0.0f);
}

void URAC_AttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	// 어떤 스탯이 변했는지 확인
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth())); // 체력 0~최대체력 사이로 제한
		OnHealthChanged.Broadcast(0.f,GetHealth());
	}
	if (Data.EvaluatedData.Attribute == GetBoltsAttribute())
	{
		SetBolts(FMath::Max(0.0f, GetBolts()));						// 볼트 음수 방지
		OnBoltsChanged.Broadcast(0.f,GetBolts());
	}

	if (Data.EvaluatedData.Attribute == GetAmmoAttribute())
	{
		SetAmmo(FMath::Clamp(GetAmmo(), 0.0f, GetMaxAmmo()));		// 탄약 0~최대탄약 사이로 제한
		OnAmmoChanged.Broadcast(0.f, GetAmmo());
	}

	if (Data.EvaluatedData.Attribute == GetMaxAmmoAttribute())
	{
		SetMaxAmmo(FMath::Max(0.0f, GetMaxAmmo()));					// 최대탄약 음수 방지
		SetAmmo(FMath::Clamp(GetAmmo(), 0.0f, GetMaxAmmo()));		// 탄약 0~최대탄약 사이로 제한	
		OnAmmoChanged.Broadcast(0.f, GetAmmo());
	}
	
}
