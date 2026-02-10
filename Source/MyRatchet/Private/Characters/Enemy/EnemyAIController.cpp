// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Enemy/EnemyAIController.h"
#include "Characters/Enemy/EnemyCharacterBase.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

// 블랙보드 키 이름 정의
const FName AEnemyAIController::TargetActorKey(TEXT("TargetActor"));
const FName AEnemyAIController::HomeLocationKey(TEXT("HomeLocation"));
const FName AEnemyAIController::HasLineOfSightKey(TEXT("bHasLineOfSight"));
const FName AEnemyAIController::AttackRangeKey(TEXT("AttackRange"));

AEnemyAIController::AEnemyAIController()
{
	// 1. Perception 컴포넌트 생성
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
	// 1-1. Blackboard 컴포넌트 생성
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));

	// 2. 시야(Sight) 설정
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = SightRadius;							// 감지 거리
	SightConfig->LoseSightRadius = LoseSightRadius;						// 감지 해제 거리
	SightConfig->PeripheralVisionAngleDegrees = PeripheralVisionAngleDegrees;			// 시야각
	SightConfig->DetectionByAffiliation.bDetectEnemies = bDetectEnemies;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = bDetectFriendlies;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = bDetectNeutrals;

	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());

	// 3. 감지 업데이트 이벤트 바인딩
	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnTargetDetected);
}

void AEnemyAIController::OnPossess(APawn* PossessedPawn)
{
	Super::OnPossess(PossessedPawn);
	if (BBAsset && BlackboardComponent)
	{
		UseBlackboard(BBAsset, BlackboardComponent);
	}

	AEnemyCharacterBase* Enemy = Cast<AEnemyCharacterBase>(PossessedPawn);
	if (Enemy && BlackboardComponent)
	{
		// 캐릭터의 공격 사거리를 BB에 복사
		BlackboardComponent->SetValueAsFloat(AttackRangeKey, Enemy->GetAttackRange());
	}

	if (BTAsset)
	{
		// 홈 위치 기록
		BlackboardComponent->SetValueAsVector(HomeLocationKey, PossessedPawn->GetActorLocation());
		RunBehaviorTree(BTAsset);
	}
}

void AEnemyAIController::OnTargetDetected(AActor* Actor, FAIStimulus Stimulus)
{
	// 감지 대상이 플레이어인지 확인
	if (Actor->ActorHasTag(TEXT("Player")))
	{
		if (!BlackboardComponent)
		{
			return;
		}

		if (Stimulus.WasSuccessfullySensed())
		{
			// 플레이어 감지
			BlackboardComponent->SetValueAsObject(TargetActorKey, Actor);
			BlackboardComponent->SetValueAsBool(HasLineOfSightKey, true);
		}
		else
		{
			// 플레이어 시야 상실
			BlackboardComponent->SetValueAsObject(TargetActorKey, nullptr);
			BlackboardComponent->SetValueAsBool(HasLineOfSightKey, false);
		}
	}
}
