// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Enemy/BTTask_ActiveAbility.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

UBTTask_ActiveAbility::UBTTask_ActiveAbility()
{
	NodeName = TEXT("Activate Ability");
}

EBTNodeResult::Type UBTTask_ActiveAbility::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	APawn* AIPawn = AIController->GetPawn();
	// 캐릭터가 IAbilitySystemInterface를 상속받았는지 확인
	IAbilitySystemInterface* ASCOwner = Cast<IAbilitySystemInterface>(AIPawn);
	
	if (ASCOwner)
	{
		UAbilitySystemComponent* ASC = ASCOwner->GetAbilitySystemComponent();
		if (ASC && AbilityTag.IsValid())
		{
			// 지정된 태그를 가진 능력을 실행 시도
			if (ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(AbilityTag)))
			{
				return EBTNodeResult::Succeeded;
			}
		}
	}

	return EBTNodeResult::Failed;
}
