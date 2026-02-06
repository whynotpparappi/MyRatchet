// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GameplayTagContainer.h"
#include "BTTask_ActiveAbility.generated.h"

/**
 * 
 */
UCLASS()
class MYRATCHET_API UBTTask_ActiveAbility : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_ActiveAbility();
	
protected:
	// Task 실행 시 호출되는 핵심 함수
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	// 에디터에서 어떤 공격 능력을 쓸지 선택할 태그
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS")
	FGameplayTag AbilityTag;
	
};
