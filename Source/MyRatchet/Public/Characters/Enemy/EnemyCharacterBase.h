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

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS", meta = (AllowPrivateAccess = "true"))
	UEnemyAbilitySystemComponentBase* AbilitySystemComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "GAS")
	bool bAbilitiesGranted = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy")
	EEnemyType EnemyType = EEnemyType::Melee;

};
