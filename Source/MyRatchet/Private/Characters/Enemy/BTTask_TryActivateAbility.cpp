#include "Characters/Enemy/BTTask_TryActivateAbility.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

UBTTask_TryActivateAbility::UBTTask_TryActivateAbility()
{
    NodeName = TEXT("Try Activate Ability");
}

EBTNodeResult::Type UBTTask_TryActivateAbility::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        return bSucceedWhenNotActivated ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
    }

    APawn* AIPawn = AIController->GetPawn();
    IAbilitySystemInterface* ASCOwner = Cast<IAbilitySystemInterface>(AIPawn);

    if (ASCOwner)
    {
        UAbilitySystemComponent* ASC = ASCOwner->GetAbilitySystemComponent();
        if (ASC && AbilityTag.IsValid())
        {
            if (ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(AbilityTag)))
            {
                return EBTNodeResult::Succeeded;
            }
        }
    }

    return bSucceedWhenNotActivated ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}
