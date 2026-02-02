// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/RAC_WeaponManager.h"
#include "Weapons/RAC_WeaponData.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "Characters/RAC_AttributeSet.h"


void URAC_WeaponManager::SwapWeapon(URAC_WeaponData* NewWeaponData)
{
	if (!NewWeaponData || NewWeaponData == CurrentWeaponData)
		return;
	
	UAbilitySystemComponent* ASC = GetASC();
	
	if (!ASC)
		return;
	
	URAC_AttributeSet* Attributes = const_cast<URAC_AttributeSet*>(
		Cast<URAC_AttributeSet>(ASC->GetAttributeSet(URAC_AttributeSet::StaticClass()))
			);
	if (!Attributes)
		return;
	
	// 1. 기본 무기 정리
	if (CurrentWeaponData)
	{
		//탄약 캐싱
		FWeaponAmmoCache& Cache = WeaponAmmoCacheMap.FindOrAdd(CurrentWeaponData->WeaponTag);
		Cache.Ammo = Attributes->GetAmmo();
		
		// 기존 능력 제거
		for (const FGameplayAbilitySpecHandle& Handle : CurrentAbilityHandles)
		{
			ASC->ClearAbility(Handle);
		}
		CurrentAbilityHandles.Empty();
		
		//기존 스탯 효과(MaxAmmo 등) 제거
		if (CurrentStatEffectHandle.IsValid())
		{
			ASC->RemoveActiveGameplayEffect(CurrentStatEffectHandle);
		}
		
	}
	
	// 2. 새 무기 비주얼 적용
	// 
	
	// 3. 새 무기 스탯 GE 적용 (MaxAmmo 등 )
	if (NewWeaponData->StatModifierEffect)
	{
		FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(NewWeaponData->StatModifierEffect, 1.0f, Context);
		if (SpecHandle.IsValid())
		{
			CurrentStatEffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
	
	// 4. 탄약 복구 (가이드 : 탄약 적용 -> ability 적용 순서 준수)
	if (WeaponAmmoCacheMap.Contains(NewWeaponData->WeaponTag))
	{
		Attributes->SetAmmo(WeaponAmmoCacheMap[NewWeaponData->WeaponTag].Ammo);
	}
	else
	{
		Attributes->SetAmmo(NewWeaponData->DefaultAmmo);
	}
	
	// 5. 새 무기 능력 부여
	for (const TSubclassOf<UGameplayAbility>& AbilityClass : NewWeaponData->AbilitiesToGrant)
	{
		if (AbilityClass)
		{
			FGameplayAbilitySpec Spec(AbilityClass, 1);
			CurrentAbilityHandles.Add(ASC->GiveAbility(Spec));
		}
	}
	CurrentWeaponData = NewWeaponData;
}

UAbilitySystemComponent* URAC_WeaponManager::GetASC() const
{
	AActor* Owner = GetOwner();
	if (!Owner) return nullptr;

	// 2. 가이드에 따라 Character나 PlayerState가 IAbilitySystemInterface를 상속받았으므로 인터페이스로 접근합니다.
	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Owner);
	if (ASI)
	{
		return ASI->GetAbilitySystemComponent();
	}

	return nullptr;
}
