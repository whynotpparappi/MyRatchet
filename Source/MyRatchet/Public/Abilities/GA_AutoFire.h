// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_AutoFire.generated.h"

class UGameplayEffect;

UCLASS()
class MYRATCHET_API UGA_AutoFire : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_AutoFire();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Fire")
	float FireInterval = 0.1f;

	UPROPERTY(EditDefaultsOnly, Category = "Fire")
	TSubclassOf<UGameplayEffect> AmmoCostEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Fire")
	bool bFireImmediately = true;
	
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<AActor> ProjectileClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float ProjectileSpeed = 3000.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	FName MuzzleSocketName = "Muzzle_01";
	

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	) override;

private:
	FTimerHandle FireTimerHandle;

	void StartFiring();
	void StopFiring();
	void FireOnce();
	bool ApplyAmmoCost();
	bool GetCrosshairRay(APlayerController* PC, FVector& OutStart, FVector& OutDir) const;
	bool GetAimTargetPoint(APlayerController* PC, AActor* IgnoreActor, float TraceDistance, FVector& OutTargetPoint) const;
	
	
};
