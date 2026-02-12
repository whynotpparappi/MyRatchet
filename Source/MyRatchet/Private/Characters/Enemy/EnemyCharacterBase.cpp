// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Enemy/EnemyCharacterBase.h"
#include "Characters/Enemy/EnemyAbilitySystemComponentBase.h"
#include "Characters/Enemy/EnemyAIController.h"
#include "Characters/RAC_AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"
#include "Abilities/GameplayAbility.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AEnemyCharacterBase::AEnemyCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponent = CreateDefaultSubobject<UEnemyAbilitySystemComponentBase>(TEXT("AbilitySystemComponent"));
	AttributeSet = CreateDefaultSubobject<URAC_AttributeSet>(TEXT("AttributeSet"));

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->SetIsReplicated(true);
		AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	}

	AIControllerClass = AEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	CurrentHealth = MaxHealth;

}

// Called when the game starts or when spawned
void AEnemyCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	InitializeAbilitySystem();
	CurrentHealth = MaxHealth;
	
}

UAbilitySystemComponent* AEnemyCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AEnemyCharacterBase::InitializeAbilitySystem()
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		if (HasAuthority())
		{
			AbilitySystemComponent->SetNumericAttributeBase(URAC_AttributeSet::GetMaxHealthAttribute(), MaxHealth);
			AbilitySystemComponent->SetNumericAttributeBase(URAC_AttributeSet::GetHealthAttribute(), MaxHealth);
		}

		if (!bHealthChangeBound)
		{
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(URAC_AttributeSet::GetHealthAttribute())
				.AddUObject(this, &AEnemyCharacterBase::HandleHealthChanged);
			bHealthChangeBound = true;
		}

		CurrentHealth = AbilitySystemComponent->GetNumericAttribute(URAC_AttributeSet::GetHealthAttribute());

		if (!bAbilitiesGranted && HasAuthority())
		{
			for (const TSubclassOf<UGameplayAbility>& AbilityClass : DefaultAbilities)
			{
				if (*AbilityClass)
				{
					FGameplayAbilitySpec Spec(AbilityClass, 1);
					AbilitySystemComponent->GiveAbility(Spec);
				}
			}

			bAbilitiesGranted = true;
		}
	}
}

// Called every frame
void AEnemyCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float AEnemyCharacterBase::TakeDamage(
	float DamageAmount,
	FDamageEvent const& DamageEvent,
	AController* EventInstigator,
	AActor* DamageCauser)
{
	if (bIsDead || DamageAmount <= 0.0f)
	{
		return 0.0f;
	}

	const float AppliedDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	const float EffectiveDamage = (AppliedDamage > 0.0f) ? AppliedDamage : DamageAmount;

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->ApplyModToAttribute(URAC_AttributeSet::GetHealthAttribute(), EGameplayModOp::Additive, -EffectiveDamage);
		CurrentHealth = AbilitySystemComponent->GetNumericAttribute(URAC_AttributeSet::GetHealthAttribute());
	}
	else
	{
		CurrentHealth = FMath::Clamp(CurrentHealth - EffectiveDamage, 0.0f, MaxHealth);
	}

	if (CurrentHealth <= 0.0f)
	{
		HandleDeath();
	}

	return EffectiveDamage;
}

// Called to bind functionality to input
void AEnemyCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyCharacterBase::KillEnemy()
{
	if (bIsDead)
	{
		return;
	}

	CurrentHealth = 0.0f;
	HandleDeath();
}

void AEnemyCharacterBase::HandleHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	CurrentHealth = ChangeData.NewValue;
	if (CurrentHealth <= 0.0f)
	{
		HandleDeath();
	}
}

void AEnemyCharacterBase::HandleDeath()
{
	if (bIsDead)
	{
		return;
	}

	bIsDead = true;

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->CancelAllAbilities();
	}

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->DisableMovement();
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (AEnemyAIController* EnemyController = Cast<AEnemyAIController>(GetController()))
	{
		EnemyController->HandleControlledPawnDeath();
	}

	bool bPlayedDeathMontage = false;
	if (DeathMontage)
	{
		if (USkeletalMeshComponent* MeshComp = GetMesh())
		{
			if (UAnimInstance* AnimInstance = MeshComp->GetAnimInstance())
			{
				const float PlayedLength = AnimInstance->Montage_Play(DeathMontage);
				if (PlayedLength > 0.0f)
				{
					FOnMontageEnded EndDelegate;
					EndDelegate.BindUObject(this, &AEnemyCharacterBase::OnDeathMontageEnded);
					AnimInstance->Montage_SetEndDelegate(EndDelegate, DeathMontage);
					bPlayedDeathMontage = true;
				}
			}
		}
	}

	if (!bPlayedDeathMontage)
	{
		SetLifeSpan(DeathDespawnDelay);
	}
}

void AEnemyCharacterBase::OnDeathMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == DeathMontage && DeathDespawnDelay >= 0.0f)
	{
		SetLifeSpan(DeathDespawnDelay);
	}
}
