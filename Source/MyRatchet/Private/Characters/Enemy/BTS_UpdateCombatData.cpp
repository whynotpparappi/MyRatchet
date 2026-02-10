#include "Characters/Enemy/BTS_UpdateCombatData.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

UBTS_UpdateCombatData::UBTS_UpdateCombatData()
{
    NodeName = TEXT("Update Combat Data");
    bNotifyTick = true;

    // 전투 갱신 주기(기본값).
    Interval = 0.25f;
    RandomDeviation = 0.05f;

    TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTS_UpdateCombatData, TargetActorKey), AActor::StaticClass());
    DistanceToTargetKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UBTS_UpdateCombatData, DistanceToTargetKey));
    HasLineOfSightKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTS_UpdateCombatData, HasLineOfSightKey));
    LastKnownLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTS_UpdateCombatData, LastKnownLocationKey));
    TimeSinceLastSeenKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UBTS_UpdateCombatData, TimeSinceLastSeenKey));
}

void UBTS_UpdateCombatData::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    AAIController* AIController = OwnerComp.GetAIOwner();
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    APawn* Pawn = AIController ? AIController->GetPawn() : nullptr;

    if (!AIController || !BlackboardComp || !Pawn)
    {
        return;
    }

    AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName));
    if (!TargetActor)
    {
        // 타겟이 없으면 시야 플래그만 끄고 종료.
        BlackboardComp->SetValueAsBool(HasLineOfSightKey.SelectedKeyName, false);
        return;
    }

    const FVector PawnLocation = Pawn->GetActorLocation();
    const FVector TargetLocation = TargetActor->GetActorLocation();

    const float Distance = FVector::Dist(PawnLocation, TargetLocation);
    BlackboardComp->SetValueAsFloat(DistanceToTargetKey.SelectedKeyName, Distance);

    const bool bHasLOS = AIController->LineOfSightTo(TargetActor);
    BlackboardComp->SetValueAsBool(HasLineOfSightKey.SelectedKeyName, bHasLOS);

    if (bHasLOS)
    {
        // 시야 확보 시 마지막 위치/경과시간 갱신.
        BlackboardComp->SetValueAsVector(LastKnownLocationKey.SelectedKeyName, TargetLocation);
        BlackboardComp->SetValueAsFloat(TimeSinceLastSeenKey.SelectedKeyName, 0.0f);
    }
    else
    {
        // 시야 상실 시 경과시간 누적.
        const float PrevTime = BlackboardComp->GetValueAsFloat(TimeSinceLastSeenKey.SelectedKeyName);
        BlackboardComp->SetValueAsFloat(TimeSinceLastSeenKey.SelectedKeyName, PrevTime + DeltaSeconds);
    }
}
