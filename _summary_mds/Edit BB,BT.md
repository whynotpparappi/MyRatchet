# Edit BB,BT

## 개요
Enemy AI의 Behavior Tree/Blackboard 갱신과 함께, 전투 관련 BT Service/Task를 추가하고 공격 사거리 블랙보드 연동을 보강한 커밋.

## 주요 변경 사항 (요약)
- BT Service/Task 3종 추가: 전투 데이터 갱신, 포커스 해제, 이동 중지.
- Enemy AIController에서 공격 사거리(AttackRange)를 블랙보드에 복사.
- EnemyCharacterBase에 공격 사거리 값을 추가하고 외부 조회 함수를 제공.
- BT/BB 에셋 교체 및 관련 애니메이션/몽타주/리타게터 에셋 추가.
- VisualStudioTools 플러그인에 MarketplaceURL 메타 추가.

## 변경 파일 (코드/설정)
- [Source/MyRatchet/Private/Characters/Enemy/BTS_UpdateCombatData.cpp](Source/MyRatchet/Private/Characters/Enemy/BTS_UpdateCombatData.cpp)
- [Source/MyRatchet/Public/Characters/Enemy/BTS_UpdateCombatData.h](Source/MyRatchet/Public/Characters/Enemy/BTS_UpdateCombatData.h)
- [Source/MyRatchet/Private/Characters/Enemy/BTTask_ClearFocus.cpp](Source/MyRatchet/Private/Characters/Enemy/BTTask_ClearFocus.cpp)
- [Source/MyRatchet/Public/Characters/Enemy/BTTask_ClearFocus.h](Source/MyRatchet/Public/Characters/Enemy/BTTask_ClearFocus.h)
- [Source/MyRatchet/Private/Characters/Enemy/BTTask_StopMovement.cpp](Source/MyRatchet/Private/Characters/Enemy/BTTask_StopMovement.cpp)
- [Source/MyRatchet/Public/Characters/Enemy/BTTask_StopMovement.h](Source/MyRatchet/Public/Characters/Enemy/BTTask_StopMovement.h)
- [Source/MyRatchet/Private/Characters/Enemy/EnemyAIController.cpp](Source/MyRatchet/Private/Characters/Enemy/EnemyAIController.cpp)
- [Source/MyRatchet/Public/Characters/Enemy/EnemyAIController.h](Source/MyRatchet/Public/Characters/Enemy/EnemyAIController.h)
- [Source/MyRatchet/Public/Characters/Enemy/EnemyCharacterBase.h](Source/MyRatchet/Public/Characters/Enemy/EnemyCharacterBase.h)
- [MyRatchet.uproject](MyRatchet.uproject)

## 변경 파일 (에셋)
- [Content/_M_RAC/Character/Enemy/BB_Enemy.uasset](Content/_M_RAC/Character/Enemy/BB_Enemy.uasset)
- [Content/_M_RAC/Character/Enemy/BT_Enemy.uasset](Content/_M_RAC/Character/Enemy/BT_Enemy.uasset)
- [Content/_M_RAC/Character/Enemy/BB_Enemy_Base.uasset](Content/_M_RAC/Character/Enemy/BB_Enemy_Base.uasset)
- [Content/_M_RAC/Character/Enemy/BT_Enemy_Base.uasset](Content/_M_RAC/Character/Enemy/BT_Enemy_Base.uasset)
- [Content/_M_RAC/Anim/Custom_Animetion/IK_Character.uasset](Content/_M_RAC/Anim/Custom_Animetion/IK_Character.uasset)
- [Content/_M_RAC/Anim/Custom_Animetion/IK_Melee.uasset](Content/_M_RAC/Anim/Custom_Animetion/IK_Melee.uasset)
- [Content/_M_RAC/Anim/Custom_Animetion/IK_Retargeter_Melee.uasset](Content/_M_RAC/Anim/Custom_Animetion/IK_Retargeter_Melee.uasset)
- [Content/_M_RAC/Anim/Custom_Animetion/Primary_Attack_A_Fast.uasset](Content/_M_RAC/Anim/Custom_Animetion/Primary_Attack_A_Fast.uasset)
- [Content/_M_RAC/Anim/Custom_Animetion/Primary_Attack_A_Medium_Montage.uasset](Content/_M_RAC/Anim/Custom_Animetion/Primary_Attack_A_Medium_Montage.uasset)
- [Content/_M_RAC/Character/Enemy/BP_EnemyAIController.uasset](Content/_M_RAC/Character/Enemy/BP_EnemyAIController.uasset)
- [Content/Map/Basic_Map/5/LN/QCX55YDL7MQ2H0I7IYVT5I.uasset](Content/Map/Basic_Map/5/LN/QCX55YDL7MQ2H0I7IYVT5I.uasset)

## 코드 변경 상세
- 전투 정보 갱신 서비스 추가
  - `UBTS_UpdateCombatData`는 대상/거리/시야/마지막 위치/경과시간을 블랙보드에 주기적으로 반영.
  - `TickNode()`에서 타겟 유무를 확인하고, 시야 유지 여부에 따라 마지막 위치와 경과시간을 갱신.
- BT Task 2종 추가
  - `UBTTask_ClearFocus`는 `AIController->ClearFocus(EAIFocusPriority::Gameplay)`로 포커스 해제.
  - `UBTTask_StopMovement`는 `AIController->StopMovement()`로 이동 즉시 중지.
- 공격 사거리 블랙보드 연동
  - `AEnemyAIController::OnPossess()`에서 `AEnemyCharacterBase::GetAttackRange()` 값을 `AttackRange` 키에 저장.
  - `AEnemyCharacterBase`에 `AttackRange` 프로퍼티와 `GetAttackRange()` 인라인 접근자 추가.
- 프로젝트 설정
  - VisualStudioTools 플러그인에 `MarketplaceURL` 메타 추가.

## 파일/함수 역할 정리
- [Source/MyRatchet/Public/Characters/Enemy/BTS_UpdateCombatData.h](Source/MyRatchet/Public/Characters/Enemy/BTS_UpdateCombatData.h)
  - `UBTS_UpdateCombatData`: 전투 상태를 주기적으로 평가해 블랙보드 키를 갱신하는 BT Service.
  - `TargetActorKey`, `DistanceToTargetKey`, `HasLineOfSightKey`, `LastKnownLocationKey`, `TimeSinceLastSeenKey`: 전투 판단에 필요한 공유 데이터 키.
- [Source/MyRatchet/Private/Characters/Enemy/BTS_UpdateCombatData.cpp](Source/MyRatchet/Private/Characters/Enemy/BTS_UpdateCombatData.cpp)
  - `UBTS_UpdateCombatData::TickNode()`: AIController/Blackboard/Pawn 확인 후 거리와 시야 및 마지막 인지 정보를 업데이트.
- [Source/MyRatchet/Public/Characters/Enemy/BTTask_ClearFocus.h](Source/MyRatchet/Public/Characters/Enemy/BTTask_ClearFocus.h)
  - `UBTTask_ClearFocus`: AI 포커스를 해제하는 BT Task.
- [Source/MyRatchet/Private/Characters/Enemy/BTTask_ClearFocus.cpp](Source/MyRatchet/Private/Characters/Enemy/BTTask_ClearFocus.cpp)
  - `UBTTask_ClearFocus::ExecuteTask()`: 포커스 해제 실행 후 성공/실패 반환.
- [Source/MyRatchet/Public/Characters/Enemy/BTTask_StopMovement.h](Source/MyRatchet/Public/Characters/Enemy/BTTask_StopMovement.h)
  - `UBTTask_StopMovement`: 이동을 즉시 취소하는 BT Task.
- [Source/MyRatchet/Private/Characters/Enemy/BTTask_StopMovement.cpp](Source/MyRatchet/Private/Characters/Enemy/BTTask_StopMovement.cpp)
  - `UBTTask_StopMovement::ExecuteTask()`: 이동 중지 실행 후 성공/실패 반환.
- [Source/MyRatchet/Public/Characters/Enemy/EnemyAIController.h](Source/MyRatchet/Public/Characters/Enemy/EnemyAIController.h)
  - `AEnemyAIController`: Perception, BB, BT 구동을 담당하는 적 AI 컨트롤러.
  - `TargetActorKey`, `HomeLocationKey`, `HasLineOfSightKey`, `AttackRangeKey`: BT/BB 공유 키 이름.
- [Source/MyRatchet/Private/Characters/Enemy/EnemyAIController.cpp](Source/MyRatchet/Private/Characters/Enemy/EnemyAIController.cpp)
  - `AEnemyAIController::OnPossess()`: 블랙보드/BT 초기화 후 공격 사거리 및 홈 위치를 기록.
  - `AEnemyAIController::OnTargetDetected()`: 시야 인지에 따라 타겟과 시야 플래그를 갱신.
- [Source/MyRatchet/Public/Characters/Enemy/EnemyCharacterBase.h](Source/MyRatchet/Public/Characters/Enemy/EnemyCharacterBase.h)
  - `AttackRange`: 공격 유효 거리(블랙보드로 전달되는 기준값).
  - `GetAttackRange()`: AIController에서 공격 사거리 조회에 사용.

## 에셋 변경 정리
- BB/BT 에셋이 Base 버전에서 최신 버전으로 교체되어 AI 상태 키 구성 및 노드 흐름이 조정됨.
- 근접 전투용 IK/리타게터/공격 몽타주 에셋 추가로 AI 공격 애니메이션 흐름 확장.
- BP_EnemyAIController 및 맵 관련 에셋이 변경되어 BT/BB 적용 및 테스트 환경이 갱신됨.

## 메모
- uasset는 바이너리이므로 BT/BB 노드/키의 정확한 세부 구성은 Unreal Editor에서 확인 필요.
