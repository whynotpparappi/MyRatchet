// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbility.h"
#include "Engine/DataAsset.h"
#include "RAC_WeaponData.generated.h"


// 1. 데이터 그룹화 (구조체)
USTRUCT()
struct FWeaponCombatSpecs
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	float DamageMultiplier = 1.0f;
	
	UPROPERTY(EditDefaultsOnly)
	float FireRate = 0.2f;
};

// 2. 에셋 파일 본체 (클래스)
UCLASS(Blueprintable)
class MYRATCHET_API URAC_WeaponData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Identity")		//무기 식별용 태그
	FGameplayTag WeaponTag;
	
	UPROPERTY(EditDefaultsOnly, Category = "GAS")			// 가이드 : 무기별 부여할 능력 목록
	TArray<TSubclassOf<UGameplayAbility>> Abilities;
	
	UPROPERTY(EditDefaultsOnly, Category = "GAS")			// 가이드 : MaxAmmo 등을 변경할 GamePlayEffect
	TSubclassOf<UGameplayEffect> StatModifierEffect;
	
	UPROPERTY(EditDefaultsOnly, Category = "Visual")		// 비주얼 : 메쉬
	TSoftObjectPtr<USkeletalMesh> WeaponMesh;
	
	UPROPERTY(EditDefaultsOnly, Category = "Visual")		// 비주얼 : 메테리얼
	TSoftClassPtr<UMaterialInstance> WeaponMaterial;
	
};
