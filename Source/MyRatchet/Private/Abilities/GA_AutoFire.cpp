// Fill out your copyright notice in the Description page of Project Settings.

#include "Abilities/GA_AutoFire.h"

#include "AbilitySystemComponent.h"
#include "Characters/RAC_AttributeSet.h"
#include "GameplayTagContainer.h"
#include "TimerManager.h"
#include "Effects/GE_AmmoCost.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"


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
	
	UWorld* World = GetWorld();
	AActor* Avatar = GetAvatarActorFromActorInfo();
	APlayerController* PC = CurrentActorInfo ? CurrentActorInfo->PlayerController.Get() : nullptr;
	
	if (!World || !Avatar || !PC || !ProjectileClass)
		return;
	
	// 1) 화면 중앙 목표점
	FVector TargetPoint;
	const float AimTraceDistance = 50000.f;
	if (!GetAimTargetPoint(PC, Avatar, AimTraceDistance, TargetPoint))
		return;

	// (디버그) 조준 라인트레이스 시각화
	FVector AimStart;
	FVector AimDir;
	if (GetCrosshairRay(PC, AimStart, AimDir))
	{
		const FVector AimEnd = AimStart + AimDir * AimTraceDistance;
		DrawDebugLine(World, AimStart, TargetPoint, FColor::Green, false, 0.05f, 0, 1.0f);
		DrawDebugPoint(World, TargetPoint, 6.0f, FColor::Red, false, 0.05f);
	}

	// 2) 총구 트랜스폼 (너 구조에 맞게 “무기 메쉬의 소켓”에서 가져오면 됨)
	FTransform MuzzleTM;
	if (USkeletalMeshComponent* MeshComp = Avatar->FindComponentByClass<USkeletalMeshComponent>())
	{
		if (MeshComp->DoesSocketExist(MuzzleSocketName))
		{
			MuzzleTM = MeshComp->GetSocketTransform(MuzzleSocketName, RTS_World);
		}
		else
		{
			MuzzleTM = Avatar->GetActorTransform();
		}
	}
	else
	{
		MuzzleTM = Avatar->GetActorTransform();
	}

	const FVector MuzzleLoc = MuzzleTM.GetLocation();

	// 3) 총구에서 목표점을 향한 방향으로 회전
	FVector ShootDir = (TargetPoint - MuzzleLoc).GetSafeNormal();
	FRotator SpawnRot = ShootDir.Rotation();

	// (옵션) 너무 가까운 벽에 총구가 박혀있을 때 보정하고 싶으면
	// MuzzleLoc를 약간 뒤로/앞으로 밀거나, 두 번째 트레이스로 SpawnLoc 조정

	FActorSpawnParameters Params;
	Params.Owner = Avatar;
	Params.Instigator = Cast<APawn>(Avatar);
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AActor* Projectile = World->SpawnActor<AActor>(ProjectileClass, MuzzleLoc, SpawnRot, Params);
	if (!Projectile)
		return;

	if (UProjectileMovementComponent* PMC = Projectile->FindComponentByClass<UProjectileMovementComponent>())
	{
		PMC->Velocity = ShootDir * ProjectileSpeed;
		PMC->Activate(true);
	}
}

bool UGA_AutoFire::GetCrosshairRay(APlayerController* PC, FVector& OutStart, FVector& OutDir) const
{
	if (!PC)
	{
		return false;
	}

	int32 SizeX = 0;
	int32 SizeY = 0;
	PC->GetViewportSize(SizeX, SizeY);
	if (SizeX <= 0 || SizeY <= 0)
	{
		return false;
	}

	const float ScreenX = SizeX * 0.5f;
	const float ScreenY = SizeY * 0.5f;

	FVector WorldLoc;
	FVector WorldDir;
	if (!PC->DeprojectScreenPositionToWorld(ScreenX, ScreenY, WorldLoc, WorldDir))
	{
		return false;
	}

	OutStart = WorldLoc;
	OutDir = WorldDir.GetSafeNormal();
	return true;
}

bool UGA_AutoFire::GetAimTargetPoint(APlayerController* PC, AActor* IgnoreActor, float TraceDistance, FVector& OutTargetPoint) const
{
	if (!PC || !PC->GetWorld())
	{
		return false;
	}

	FVector Start;
	FVector Dir;
	if (!GetCrosshairRay(PC, Start, Dir))
	{
		return false;
	}

	const FVector End = Start + Dir * TraceDistance;

	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(AimTrace), true, IgnoreActor);
	if (IgnoreActor)
	{
		Params.AddIgnoredActor(IgnoreActor);
	}

	const bool bHit = PC->GetWorld()->LineTraceSingleByChannel(
		Hit,
		Start,
		End,
		AimTraceChannel,
		Params
	);

	OutTargetPoint = bHit ? Hit.ImpactPoint : End;
	return true;
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
