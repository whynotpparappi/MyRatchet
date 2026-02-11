// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "EnemyAIController.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UBlackboardComponent;
class UBlackboardData;
class UBehaviorTree;

/**
 * 
 */
UCLASS()
class MYRATCHET_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyAIController();

protected:
	virtual void OnPossess(APawn* PossessedPawn) override;

	UFUNCTION()
	void OnTargetDetected(AActor* Actor, FAIStimulus Stimulus);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UAIPerceptionComponent* AIPerceptionComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UBlackboardComponent* BlackboardComponent = nullptr;

	UPROPERTY()
	UAISenseConfig_Sight* SightConfig = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Sight", meta = (ClampMin = "0.0"))
	float SightRadius = 2000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Sight", meta = (ClampMin = "0.0"))
	float LoseSightRadius = 2500.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Sight", meta = (ClampMin = "0.0", ClampMax = "180.0"))
	float PeripheralVisionAngleDegrees = 60.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Sight")
	bool bDetectEnemies = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Sight")
	bool bDetectFriendlies = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Sight")
	bool bDetectNeutrals = true;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UBlackboardData* BBAsset = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UBehaviorTree* BTAsset = nullptr;

	// 블랙보드 키 이름들
	static const FName TargetActorKey;
	static const FName HomeLocationKey;
	static const FName HasLineOfSightKey;
	// 공격 사거리 키
	static const FName AttackRangeKey;
	// 원거리 공격 사거리 키
	static const FName RangedAttackRangeKey;
};
