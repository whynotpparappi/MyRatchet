#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_EnemyRangedAttack.generated.h"

class USoundBase;

UCLASS()
class MYRATCHET_API UGA_EnemyRangedAttack : public UGameplayAbility
{
    GENERATED_BODY()

public:
    UGA_EnemyRangedAttack();

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Projectile")
    TSubclassOf<AActor> ProjectileClass;

    UPROPERTY(EditDefaultsOnly, Category = "Projectile")
    float ProjectileSpeed = 3000.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Projectile", meta = (ClampMin = "0.0"))
    float ProjectileDamage = 15.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Projectile", meta = (ClampMin = "0.0"))
    float SpawnForwardOffset = 25.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Projectile")
    FName MuzzleSocketName = "Muzzle_Enemy";

    // Static Mesh 컴포넌트 이름(없으면 아무 StaticMeshComponent나 탐색)
    UPROPERTY(EditDefaultsOnly, Category = "Projectile")
    FName WeaponMeshComponentName = "";

    // 블랙보드에서 타겟을 읽어올 키 이름
    UPROPERTY(EditDefaultsOnly, Category = "Target")
    FName TargetActorKey = "TargetActor";

    // 타겟 조준 위치 보정(필요 시)
    UPROPERTY(EditDefaultsOnly, Category = "Target")
    float TargetLocationZOffset = 0.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Audio")
    TObjectPtr<USoundBase> FireSound = nullptr;

    UPROPERTY(EditDefaultsOnly, Category = "Audio", meta = (ClampMin = "0.0"))
    float FireSoundVolume = 1.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Audio", meta = (ClampMin = "0.1"))
    float FireSoundPitch = 1.0f;

    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData
    ) override;

private:
    AActor* GetTargetActorFromBlackboard(APawn* Pawn) const;
    bool GetMuzzleTransform(AActor* Avatar, FTransform& OutTransform) const;
};
