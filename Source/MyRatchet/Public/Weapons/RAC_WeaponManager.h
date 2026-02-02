// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayAbilitySpec.h"		//FGameplayAbilitySpecHandle 사용을 위해 필요
#include "RAC_WeaponManager.generated.h"

class URAC_WeaponData;
class UAbilitySystemComponent;

USTRUCT()
struct FWeaponAmmoCache
{
	GENERATED_BODY()
	float Ammo = 0.0f;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYRATCHET_API URAC_WeaponManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	UFUNCTION( BlueprintCallable , Category="Weapon")
	void SwapWeapon(URAC_WeaponData* NewWeaponData);

protected:
	// 가이드의 탄약 캐시 맵
	UPROPERTY()
	TMap<FGameplayTag, FWeaponAmmoCache> WeaponAmmoCacheMap;

	UPROPERTY()
	URAC_WeaponData* CurrentWeaponData;

	// 현재 부여된 능력 핸들 보관 (나중에 제거하기 위함)
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> CurrentAbilityHandles;

	// 현재 적용된 속성 변경 GE 핸들 보관
	UPROPERTY()
	FActiveGameplayEffectHandle CurrentStatEffectHandle;

	UAbilitySystemComponent* GetASC() const;
	
};
