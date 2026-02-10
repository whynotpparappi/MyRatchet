#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_StopMovement.generated.h"

// AI 이동을 즉시 중지시키는 BT Task.
UCLASS()
class MYRATCHET_API UBTTask_StopMovement : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_StopMovement();

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
