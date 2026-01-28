// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"		//필수
#include "RAC_AttributeSet.generated.h"


//매크로를 적용
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
		GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
/**
 * 
 */
UCLASS()
class MYRATCHET_API URAC_AttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	URAC_AttributeSet();
	
	// 1. 생존 및 경제 (Core)
	UPROPERTY(BlueprintReadOnly, Category="Attributes-Core")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(URAC_AttributeSet, Health);			//위에서 만든 매크로 적용
	
	UPROPERTY(BlueprintReadOnly, Category="Attributes-Core")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(URAC_AttributeSet, MaxHealth);
	
	UPROPERTY(BlueprintReadOnly, Category="Attributes-Core")
	FGameplayAttributeData Bolts;
	ATTRIBUTE_ACCESSORS(URAC_AttributeSet, Bolts);
	
	UPROPERTY(BlueprintReadOnly, Category="Attributes-Core")
	FGameplayAttributeData Experience;
	ATTRIBUTE_ACCESSORS(URAC_AttributeSet, Experience);
	
	// 2. 전투 (Combat)
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData CurrentAmmo;
	ATTRIBUTE_ACCESSORS(URAC_AttributeSet, CurrentAmmo);
	
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData MaxAmmo;
	ATTRIBUTE_ACCESSORS(URAC_AttributeSet, MaxAmmo);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData DamageMultiplier;
	ATTRIBUTE_ACCESSORS(URAC_AttributeSet, DamageMultiplier);

	// --- 3. 유틸리티 (Utility) ---
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData DashCooldown;
	ATTRIBUTE_ACCESSORS(URAC_AttributeSet, DashCooldown);
	
	// 데미지 처리를 위한 특수 함수 (나중에 구현)
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
};
