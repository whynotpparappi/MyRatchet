#include "Characters/Enemy/BTTask_ClearFocus.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

UBTTask_ClearFocus::UBTTask_ClearFocus()
{
    // BT 에디터에 표시되는 노드 이름.
    NodeName = TEXT("Clear Focus");
}

EBTNodeResult::Type UBTTask_ClearFocus::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    // 컨트롤러가 없으면 실패 처리.
    if (!AIController)
    {
        return EBTNodeResult::Failed;
    }

    // Gameplay 우선순위의 포커스를 해제.
    AIController->ClearFocus(EAIFocusPriority::Gameplay);
    return EBTNodeResult::Succeeded;
}
