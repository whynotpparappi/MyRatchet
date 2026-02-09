// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Enemy/EnemyAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

// 블랙보드 키 이름 정의
const FName AEnemyAIController::TargetActorKey(TEXT("TargetActor"));
const FName AEnemyAIController::HomeLocationKey(TEXT("HomeLocation"));
const FName AEnemyAIController::HasLineOfSightKey(TEXT("bHasLineOfSight"));

AEnemyAIController::AEnemyAIController()
{
	// 1. Perception 컴포넌트 생성
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
	// 1-1. 블랙보드 컴포넌트 생성
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));

	// 2. 시야(Sight) 설정
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = SightRadius;							// 인지 사거리
	SightConfig->LoseSightRadius = LoseSightRadius;						// 인지 해제 사거리
	SightConfig->PeripheralVisionAngleDegrees = PeripheralVisionAngleDegrees;			// 시야각 (반각이므로 실제 120도)
	SightConfig->DetectionByAffiliation.bDetectEnemies = bDetectEnemies;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = bDetectFriendlies;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = bDetectNeutrals;

	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());

	// 3. 감각 업데이트 이벤트 바인딩
	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnTargetDetected);
}

void AEnemyAIController::OnPossess(APawn* PossessedPawn)
{
	Super::OnPossess(PossessedPawn);
	if (BBAsset && BlackboardComponent)
	{
		UseBlackboard(BBAsset, BlackboardComponent);
	}

	if (BTAsset)
	{
		// 홈 위치 저장
		BlackboardComponent->SetValueAsVector(HomeLocationKey, PossessedPawn->GetActorLocation());
		RunBehaviorTree(BTAsset);
	}
}

void AEnemyAIController::OnTargetDetected(AActor* Actor, FAIStimulus Stimulus)
{
	// 감지된 대상이 플레이어인지 확인 (태그나 클래스로 구분 가능)
	if (Actor->ActorHasTag(TEXT("Player")))
	{
		if (!BlackboardComponent)
		{
			return;
		}

		if (Stimulus.WasSuccessfullySensed())
		{
			// 플레이어를 시야에 포착함
			BlackboardComponent->SetValueAsObject(TargetActorKey, Actor);
			BlackboardComponent->SetValueAsBool(HasLineOfSightKey, true);
		}
		else
		{
			// 플레이어가 시야에서 사라짐
			BlackboardComponent->SetValueAsObject(TargetActorKey, nullptr);
			BlackboardComponent->SetValueAsBool(HasLineOfSightKey, false);
		}
	}
}
