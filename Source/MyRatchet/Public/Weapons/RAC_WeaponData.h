// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbility.h"
#include "Engine/DataAsset.h"
#include "RAC_WeaponData.generated.h"

class UGameplayAbility;
class UGameplayEffect;

UCLASS(Blueprintable)
class MYRATCHET_API URAC_WeaponData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Identity")		// 무기 식별용 태그
	FGameplayTag WeaponTag;
	
	UPROPERTY(EditDefaultsOnly, Category = "GAS")			// 가이드 : 무기별 부여할 능력 목록
	TArray<TSubclassOf<UGameplayAbility>> AbilitiesToGrant;
	
	UPROPERTY(EditDefaultsOnly, Category = "GAS")			// 가이드 : MaxAmmo 등을 변경할 GamePlayEffect
	TSubclassOf<UGameplayEffect> StatModifierEffect;
	
	UPROPERTY(EditDefaultsOnly, Category = "Visual")		// 비주얼 : 메쉬
	TSoftObjectPtr<USkeletalMesh> WeaponMesh;
	
	UPROPERTY(EditDefaultsOnly, Category = "Visual")		// 비주얼 : 메테리얼
	TSoftClassPtr<UMaterialInstance> WeaponMaterial;

	// 발사 사양
	UPROPERTY(EditDefaultsOnly, Category = "Combat")		//True: 연발, False: 단발
	bool bIsFullAuto;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat")		//True: 포물선, False: 직선
	bool bIsParabolic;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat")		//데미지 수치
	float DamageValue;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat")		//캐시값이 없을 때 초기값
	float DefaultAmmo;
	
	
};
