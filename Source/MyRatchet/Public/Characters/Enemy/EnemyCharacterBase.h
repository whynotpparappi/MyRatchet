// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Characters/Enemy/YourProjectTypes.h"

#include "EnemyCharacterBase.generated.h"

class UEnemyAbilitySystemComponentBase;
class AEnemyAIController;
class UGameplayAbility;
class URAC_AttributeSet;
class UAnimMontage;
struct FOnAttributeChangeData;


UCLASS()
class MYRATCHET_API AEnemyCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyCharacterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void InitializeAbilitySystem();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 공격 사거리 값을 외부에서 참조할 수 있도록 제공.
	FORCEINLINE float GetAttackRange() const { return AttackRange; }
	// 원거리 공격 사거리 값을 외부에서 참조할 수 있도록 제공.
	FORCEINLINE float GetRangedAttackRange() const { return RangedAttackRange; }
	FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }
	FORCEINLINE bool IsDead() const { return bIsDead; }

	UFUNCTION(BlueprintCallable, Category = "Enemy|Combat")
	void KillEnemy();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS", meta = (AllowPrivateAccess = "true"))
	UEnemyAbilitySystemComponentBase* AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS", meta = (AllowPrivateAccess = "true"))
	URAC_AttributeSet* AttributeSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "GAS")
	bool bAbilitiesGranted = false;

	// 근거리 공격 유효 거리(블랙보드로 복사됨).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Combat", meta = (ClampMin = "0.0"))
	float AttackRange = 200.0f;

	// 원거리 공격 유효 거리(블랙보드로 복사됨).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Combat", meta = (ClampMin = "0.0"))
	float RangedAttackRange = 1800.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy")
	EEnemyType EnemyType = EEnemyType::Melee;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Health", meta = (ClampMin = "1.0"))
	float MaxHealth = 100.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Enemy|Health")
	float CurrentHealth = 100.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Enemy|Health")
	bool bIsDead = false;

private:
	void HandleHealthChanged(const FOnAttributeChangeData& ChangeData);
	void OnDeathMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	void HandleDeath();
	bool bHealthChangeBound = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Death", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DeathMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Death", meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float DeathDespawnDelay = 2.0f;

};
