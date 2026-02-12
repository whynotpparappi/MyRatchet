#include "Abilities/GA_EnemyRangedAttack.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameplayTagContainer.h"
#include "Combat/RAC_ProjectileBase.h"

UGA_EnemyRangedAttack::UGA_EnemyRangedAttack()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    bRetriggerInstancedAbility = true;

    AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Enemy.Attack.Ranged")));
}

void UGA_EnemyRangedAttack::ActivateAbility(
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

    UWorld* World = GetWorld();
    AActor* Avatar = GetAvatarActorFromActorInfo();
    APawn* Pawn = Cast<APawn>(Avatar);

    if (!World || !Avatar || !Pawn || !ProjectileClass)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    AActor* TargetActor = GetTargetActorFromBlackboard(Pawn);
    if (!TargetActor)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    FTransform MuzzleTM;
    if (!GetMuzzleTransform(Avatar, MuzzleTM))
    {
        MuzzleTM = Avatar->GetActorTransform();
    }

    const FVector MuzzleLoc = MuzzleTM.GetLocation();
    FVector TargetLoc = TargetActor->GetActorLocation();
    TargetLoc.Z += TargetLocationZOffset;

    FVector ShootDir = (TargetLoc - MuzzleLoc).GetSafeNormal();
    if (ShootDir.IsNearlyZero())
    {
        ShootDir = Avatar->GetActorForwardVector();
    }

    const FRotator SpawnRot = ShootDir.Rotation();
    const FVector SpawnLoc = MuzzleLoc + (ShootDir * SpawnForwardOffset);

    FActorSpawnParameters Params;
    Params.Owner = Avatar;
    Params.Instigator = Pawn;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    AActor* Projectile = World->SpawnActor<AActor>(ProjectileClass, SpawnLoc, SpawnRot, Params);
    if (Projectile)
    {
        if (UPrimitiveComponent* ProjectileRootPrimitive = Cast<UPrimitiveComponent>(Projectile->GetRootComponent()))
        {
            ProjectileRootPrimitive->IgnoreActorWhenMoving(Avatar, true);
            ProjectileRootPrimitive->IgnoreActorWhenMoving(Pawn, true);
        }

        if (ARAC_ProjectileBase* ProjectileBase = Cast<ARAC_ProjectileBase>(Projectile))
        {
            ProjectileBase->Damage = ProjectileDamage;
        }

        if (UProjectileMovementComponent* PMC = Projectile->FindComponentByClass<UProjectileMovementComponent>())
        {
            PMC->Velocity = ShootDir * ProjectileSpeed;
            PMC->Activate(true);
        }
    }

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

AActor* UGA_EnemyRangedAttack::GetTargetActorFromBlackboard(APawn* Pawn) const
{
    if (!Pawn)
    {
        return nullptr;
    }

    AAIController* AIController = Cast<AAIController>(Pawn->GetController());
    if (!AIController)
    {
        return nullptr;
    }

    UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
    if (!BlackboardComp)
    {
        return nullptr;
    }

    return Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorKey));
}

bool UGA_EnemyRangedAttack::GetMuzzleTransform(AActor* Avatar, FTransform& OutTransform) const
{
    if (!Avatar)
    {
        return false;
    }

    // 1) StaticMeshComponent에서 먼저 소켓 탐색
    UStaticMeshComponent* StaticMeshComp = nullptr;
    if (!WeaponMeshComponentName.IsNone())
    {
        TArray<UStaticMeshComponent*> MeshComps;
        Avatar->GetComponents<UStaticMeshComponent>(MeshComps);
        for (UStaticMeshComponent* Comp : MeshComps)
        {
            if (Comp && Comp->GetFName() == WeaponMeshComponentName)
            {
                StaticMeshComp = Comp;
                break;
            }
        }
    }

    if (!StaticMeshComp)
    {
        StaticMeshComp = Avatar->FindComponentByClass<UStaticMeshComponent>();
    }

    if (StaticMeshComp && StaticMeshComp->DoesSocketExist(MuzzleSocketName))
    {
        OutTransform = StaticMeshComp->GetSocketTransform(MuzzleSocketName, RTS_World);
        return true;
    }

    // 2) 스켈레탈 메시 소켓(백업)
    if (USkeletalMeshComponent* MeshComp = Avatar->FindComponentByClass<USkeletalMeshComponent>())
    {
        if (MeshComp->DoesSocketExist(MuzzleSocketName))
        {
            OutTransform = MeshComp->GetSocketTransform(MuzzleSocketName, RTS_World);
            return true;
        }
    }

    return false;
}
