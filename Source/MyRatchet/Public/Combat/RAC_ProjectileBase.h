#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RAC_ProjectileBase.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UDamageType;
class UPrimitiveComponent;

UCLASS()
class MYRATCHET_API ARAC_ProjectileBase : public AActor
{
    GENERATED_BODY()

public:
    ARAC_ProjectileBase();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
    USphereComponent* CollisionComponent = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
    UProjectileMovementComponent* ProjectileMovement = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage", meta = (ClampMin = "0.0", ExposeOnSpawn = "true"))
    float Damage = 10.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
    TSubclassOf<UDamageType> DamageTypeClass;

protected:
    virtual void BeginPlay() override;

private:
    UFUNCTION()
    void OnProjectileHit(
        UPrimitiveComponent* HitComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComponent,
        FVector NormalImpulse,
        const FHitResult& Hit);

    UFUNCTION()
    void OnProjectileBeginOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComponent,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult);

    void HandleImpact(AActor* OtherActor);

    bool bImpactProcessed = false;
};
