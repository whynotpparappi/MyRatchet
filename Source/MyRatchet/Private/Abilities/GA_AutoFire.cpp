// Fill out your copyright notice in the Description page of Project Settings.

#include "Abilities/GA_AutoFire.h"

#include "AbilitySystemComponent.h"
#include "Characters/RAC_AttributeSet.h"
#include "GameplayTagContainer.h"
#include "TimerManager.h"
#include "Effects/GE_AmmoCost.h"

UGA_AutoFire::UGA_AutoFire()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	bRetriggerInstancedAbility = true;

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Weapon.FireMode.Auto")));

	AmmoCostEffect = UGE_AmmoCost::StaticClass();
}

void UGA_AutoFire::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	StartFiring();
}

void UGA_AutoFire::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	StopFiring();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_AutoFire::StartFiring()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (bFireImmediately)
	{
		FireOnce();
	}

	World->GetTimerManager().SetTimer(
		FireTimerHandle,
		this,
		&UGA_AutoFire::FireOnce,
		FireInterval,
		true
	);
}

void UGA_AutoFire::StopFiring()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(FireTimerHandle);
	}
}

void UGA_AutoFire::FireOnce()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	const float CurrentAmmo = ASC->GetNumericAttribute(URAC_AttributeSet::GetAmmoAttribute());
	if (CurrentAmmo <= 0.0f)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	if (!ApplyAmmoCost())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	// TODO: 실제 발사(프로젝타일/라인트레이스) 로직 연결
}

bool UGA_AutoFire::ApplyAmmoCost()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC || !AmmoCostEffect)
	{
		return false;
	}

	FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(AmmoCostEffect, 1.0f, Context);
	if (!SpecHandle.IsValid())
	{
		return false;
	}

	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	return true;
}
