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
    AttackRangeKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UBTS_UpdateCombatData, AttackRangeKey));
    RangedAttackRangeKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UBTS_UpdateCombatData, RangedAttackRangeKey));
    IsInAttackRangeKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTS_UpdateCombatData, IsInAttackRangeKey));
    IsInRangedRangeKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTS_UpdateCombatData, IsInRangedRangeKey));
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
        // 타겟이 없으면 시야/공격 플래그를 끄고 종료.
        if (HasLineOfSightKey.SelectedKeyName != NAME_None)
        {
            BlackboardComp->SetValueAsBool(HasLineOfSightKey.SelectedKeyName, false);
        }
        if (IsInAttackRangeKey.SelectedKeyName != NAME_None)
        {
            BlackboardComp->SetValueAsBool(IsInAttackRangeKey.SelectedKeyName, false);
        }
        if (IsInRangedRangeKey.SelectedKeyName != NAME_None)
        {
            BlackboardComp->SetValueAsBool(IsInRangedRangeKey.SelectedKeyName, false);
        }
        return;
    }

    const FVector PawnLocation = Pawn->GetActorLocation();
    const FVector TargetLocation = TargetActor->GetActorLocation();

    const float Distance = FVector::Dist(PawnLocation, TargetLocation);
    if (DistanceToTargetKey.SelectedKeyName != NAME_None)
    {
        BlackboardComp->SetValueAsFloat(DistanceToTargetKey.SelectedKeyName, Distance);
    }

    const bool bHasLOS = AIController->LineOfSightTo(TargetActor);
    if (HasLineOfSightKey.SelectedKeyName != NAME_None)
    {
        BlackboardComp->SetValueAsBool(HasLineOfSightKey.SelectedKeyName, bHasLOS);
    }

    const float MeleeRange = (AttackRangeKey.SelectedKeyName != NAME_None)
        ? BlackboardComp->GetValueAsFloat(AttackRangeKey.SelectedKeyName)
        : 0.0f;
    const float RangedRange = (RangedAttackRangeKey.SelectedKeyName != NAME_None)
        ? BlackboardComp->GetValueAsFloat(RangedAttackRangeKey.SelectedKeyName)
        : 0.0f;

    const bool bWasInMelee = (IsInAttackRangeKey.SelectedKeyName != NAME_None)
        ? BlackboardComp->GetValueAsBool(IsInAttackRangeKey.SelectedKeyName)
        : false;
    const bool bWasInRanged = (IsInRangedRangeKey.SelectedKeyName != NAME_None)
        ? BlackboardComp->GetValueAsBool(IsInRangedRangeKey.SelectedKeyName)
        : false;

    const float MeleeExit = MeleeRange + MeleeExitBuffer;
    const float RangedExit = RangedRange + RangedExitBuffer;

    const bool bInMeleeRange = (MeleeRange > 0.0f)
        ? (bWasInMelee ? (Distance <= MeleeExit) : (Distance <= MeleeRange))
        : false;

    // 원거리 범위는 근거리와 겹치지 않도록 MeleeRange보다 큰 구간에서만 true
    const bool bInRangedRange = (RangedRange > 0.0f)
        ? (bWasInRanged ? (Distance <= RangedExit && Distance > MeleeRange)
                        : (Distance <= RangedRange && Distance > MeleeRange))
        : false;

    if (IsInAttackRangeKey.SelectedKeyName != NAME_None)
    {
        BlackboardComp->SetValueAsBool(IsInAttackRangeKey.SelectedKeyName, bInMeleeRange);
    }
    if (IsInRangedRangeKey.SelectedKeyName != NAME_None)
    {
        BlackboardComp->SetValueAsBool(IsInRangedRangeKey.SelectedKeyName, bInRangedRange);
    }

    if (bHasLOS)
    {
        // 시야 확보 시 마지막 위치/경과시간 갱신.
        if (LastKnownLocationKey.SelectedKeyName != NAME_None)
        {
            BlackboardComp->SetValueAsVector(LastKnownLocationKey.SelectedKeyName, TargetLocation);
        }
        if (TimeSinceLastSeenKey.SelectedKeyName != NAME_None)
        {
            BlackboardComp->SetValueAsFloat(TimeSinceLastSeenKey.SelectedKeyName, 0.0f);
        }
    }
    else
    {
        // 시야 상실 시 경과시간 누적.
        if (TimeSinceLastSeenKey.SelectedKeyName != NAME_None)
        {
            const float PrevTime = BlackboardComp->GetValueAsFloat(TimeSinceLastSeenKey.SelectedKeyName);
            BlackboardComp->SetValueAsFloat(TimeSinceLastSeenKey.SelectedKeyName, PrevTime + DeltaSeconds);
        }
    }
}
