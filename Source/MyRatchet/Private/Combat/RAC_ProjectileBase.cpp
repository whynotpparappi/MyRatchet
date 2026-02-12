#include "Combat/RAC_ProjectileBase.h"

#include "Characters/Enemy/EnemyCharacterBase.h"
#include "Characters/RAC_CPP_Character.h"
#include "Characters/RAC_AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"

ARAC_ProjectileBase::ARAC_ProjectileBase()
{
    PrimaryActorTick.bCanEverTick = false;

    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
    SetRootComponent(CollisionComponent);

    CollisionComponent->InitSphereRadius(10.0f);
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CollisionComponent->SetCollisionObjectType(ECC_WorldDynamic);
    CollisionComponent->SetCollisionResponseToAllChannels(ECR_Block);
    CollisionComponent->SetGenerateOverlapEvents(true);
    CollisionComponent->SetNotifyRigidBodyCollision(true);

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->InitialSpeed = 3000.0f;
    ProjectileMovement->MaxSpeed = 3000.0f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->ProjectileGravityScale = 0.0f;
}

void ARAC_ProjectileBase::BeginPlay()
{
    Super::BeginPlay();

    if (CollisionComponent)
    {
        CollisionComponent->OnComponentHit.AddDynamic(this, &ARAC_ProjectileBase::OnProjectileHit);
        CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ARAC_ProjectileBase::OnProjectileBeginOverlap);

        // Prevent immediate self-collision at muzzle spawn.
        if (AActor* OwnerActor = GetOwner())
        {
            CollisionComponent->IgnoreActorWhenMoving(OwnerActor, true);

            TArray<AActor*> AttachedActors;
            OwnerActor->GetAttachedActors(AttachedActors);
            for (AActor* AttachedActor : AttachedActors)
            {
                if (AttachedActor)
                {
                    CollisionComponent->IgnoreActorWhenMoving(AttachedActor, true);
                }
            }
        }

        if (APawn* InstigatorPawn = GetInstigator())
        {
            CollisionComponent->IgnoreActorWhenMoving(InstigatorPawn, true);
        }
    }
}

void ARAC_ProjectileBase::OnProjectileHit(
    UPrimitiveComponent* HitComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComponent,
    FVector NormalImpulse,
    const FHitResult& Hit)
{
    HandleImpact(OtherActor);
}

void ARAC_ProjectileBase::OnProjectileBeginOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComponent,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    HandleImpact(OtherActor);
}

void ARAC_ProjectileBase::HandleImpact(AActor* OtherActor)
{
    if (bImpactProcessed)
    {
        return;
    }

    if (!OtherActor || OtherActor == this || OtherActor == GetOwner() || OtherActor == GetInstigator())
    {
        return;
    }

    // Ignore same-side hits by default.
    const bool bShooterIsEnemy = (GetInstigator() && GetInstigator()->IsA(AEnemyCharacterBase::StaticClass()))
        || (GetOwner() && GetOwner()->IsA(AEnemyCharacterBase::StaticClass()));
    const bool bTargetIsEnemy = OtherActor->IsA(AEnemyCharacterBase::StaticClass());
    if (bShooterIsEnemy && bTargetIsEnemy)
    {
        return;
    }

    const bool bShooterIsPlayer = (GetInstigator() && GetInstigator()->IsA(ARAC_CPP_Character::StaticClass()))
        || (GetOwner() && GetOwner()->IsA(ARAC_CPP_Character::StaticClass()));
    const bool bTargetIsPlayer = OtherActor->IsA(ARAC_CPP_Character::StaticClass());
    if (bShooterIsPlayer && bTargetIsPlayer)
    {
        return;
    }

    bImpactProcessed = true;

    if (ARAC_ProjectileBase* OtherProjectile = Cast<ARAC_ProjectileBase>(OtherActor))
    {
        OtherProjectile->Destroy();
        Destroy();
        return;
    }

    float FinalDamage = Damage;
    IAbilitySystemInterface* SourceASI = Cast<IAbilitySystemInterface>(GetOwner());
    if (!SourceASI)
    {
        SourceASI = Cast<IAbilitySystemInterface>(GetInstigator());
    }

    if (SourceASI)
    {
        if (UAbilitySystemComponent* SourceASC = SourceASI->GetAbilitySystemComponent())
        {
            const float DamageMultiplier = SourceASC->GetNumericAttribute(URAC_AttributeSet::GetDamageMultiplierAttribute());
            FinalDamage *= FMath::Max(0.0f, DamageMultiplier);
        }
    }

    if (IAbilitySystemInterface* TargetASI = Cast<IAbilitySystemInterface>(OtherActor))
    {
        if (UAbilitySystemComponent* TargetASC = TargetASI->GetAbilitySystemComponent())
        {
            TargetASC->ApplyModToAttribute(URAC_AttributeSet::GetHealthAttribute(), EGameplayModOp::Additive, -FinalDamage);
            Destroy();
            return;
        }
    }

    AController* InstigatorController = GetInstigatorController();
    const TSubclassOf<UDamageType> ResolvedDamageType = DamageTypeClass
        ? DamageTypeClass
        : TSubclassOf<UDamageType>(UDamageType::StaticClass());

    UGameplayStatics::ApplyDamage(
        OtherActor,
        FinalDamage,
        InstigatorController,
        this,
        ResolvedDamageType);

    Destroy();
}
