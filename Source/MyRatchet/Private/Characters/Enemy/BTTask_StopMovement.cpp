#include "Characters/Enemy/BTTask_StopMovement.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

UBTTask_StopMovement::UBTTask_StopMovement()
{
    // BT 에디터에 표시되는 노드 이름.
    NodeName = TEXT("Stop Movement");
}

EBTNodeResult::Type UBTTask_StopMovement::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    // 컨트롤러가 없으면 실패 처리.
    if (!AIController)
    {
        return EBTNodeResult::Failed;
    }

    // 현재 이동 요청을 취소.
    AIController->StopMovement();
    return EBTNodeResult::Succeeded;
}
