#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTS_UpdateCombatData.generated.h"

// 전투 관련 블랙보드 값을 주기적으로 갱신하는 서비스(거리/시야/마지막 위치/경과시간).
UCLASS()
class MYRATCHET_API UBTS_UpdateCombatData : public UBTService
{
    GENERATED_BODY()

public:
    UBTS_UpdateCombatData();

protected:
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

public:
    // BT에서 매핑할 블랙보드 키들.
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector TargetActorKey;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector DistanceToTargetKey;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector HasLineOfSightKey;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector LastKnownLocationKey;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector TimeSinceLastSeenKey;
};
