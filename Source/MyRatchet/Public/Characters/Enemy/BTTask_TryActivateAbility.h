#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GameplayTagContainer.h"
#include "BTTask_TryActivateAbility.generated.h"

// 실패하더라도 트리를 끊지 않고 시도만 하는 능력 발동 Task.
UCLASS()
class MYRATCHET_API UBTTask_TryActivateAbility : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_TryActivateAbility();

    // 발동할 능력 태그
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS")
    FGameplayTag AbilityTag;

    // 발동 실패 시에도 성공으로 처리할지 여부
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS")
    bool bSucceedWhenNotActivated = true;

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
