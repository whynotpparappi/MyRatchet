# Add BB + BT

작성일: 2026-02-09

## 변경 요약
- AI 인지(Perception) + 블랙보드/비헤이비어 트리 기반 적 AI 흐름을 C++ 컨트롤러에 구현하고, BT/BB 에셋을 베이스 자산으로 정리했습니다.
- 적 캐릭터에 기본 GAS 어빌리티 부여 로직을 추가해 BT에서 능력 발동이 가능하도록 준비했습니다.
- 적 근접 공격용 태그를 GameplayTags에 추가했습니다.
- AI/BT 관련 모듈 의존성을 Build 설정에 추가했습니다.

## 코드/설정 변경 상세
### AI 컨트롤러 (Perception + BB/BT 연결)
- AEnemyAIController 생성자에서 Perception, Blackboard, Sight 설정을 초기화하고 감지 이벤트를 바인딩합니다. 시야 거리/각도/감지 대상과 같은 파라미터를 에디터에서 조정 가능한 프로퍼티로 노출합니다. [Source/MyRatchet/Private/Characters/Enemy/EnemyAIController.cpp](Source/MyRatchet/Private/Characters/Enemy/EnemyAIController.cpp#L11-L37) [Source/MyRatchet/Public/Characters/Enemy/EnemyAIController.h](Source/MyRatchet/Public/Characters/Enemy/EnemyAIController.h#L10-L69)
- OnPossess에서 BB를 적용하고 BT를 실행합니다. 동시에 스폰 지점(홈 위치)을 블랙보드에 기록해 복귀/순찰 등의 기준점으로 활용할 수 있게 합니다. [Source/MyRatchet/Private/Characters/Enemy/EnemyAIController.cpp](Source/MyRatchet/Private/Characters/Enemy/EnemyAIController.cpp#L39-L53)
- OnTargetDetected는 감지된 액터가 "Player" 태그일 때만 타깃으로 채택하고, 시야 확보 여부에 따라 TargetActor/LineOfSight 값을 업데이트합니다. BT/BB의 추적·전투 분기 조건으로 사용됩니다. [Source/MyRatchet/Private/Characters/Enemy/EnemyAIController.cpp](Source/MyRatchet/Private/Characters/Enemy/EnemyAIController.cpp#L55-L78)

### 적 캐릭터 기본 어빌리티 지급
- InitializeAbilitySystem에서 서버 권한일 때 DefaultAbilities에 등록된 어빌리티를 ASC에 부여하고, 중복 지급을 방지하기 위해 bAbilitiesGranted를 사용합니다. BTTask에서 태그 기반 능력 호출이 가능해집니다. [Source/MyRatchet/Private/Characters/Enemy/EnemyCharacterBase.cpp](Source/MyRatchet/Private/Characters/Enemy/EnemyCharacterBase.cpp#L43-L63) [Source/MyRatchet/Public/Characters/Enemy/EnemyCharacterBase.h](Source/MyRatchet/Public/Characters/Enemy/EnemyCharacterBase.h#L41-L52)

### BT Task 데이터 정리
- BTTask_ActiveAbility의 AbilityTag가 에디터 노출 프로퍼티로 유지되며, ExecuteTask는 BT에서 선택된 태그를 기반으로 GAS 능력을 발동하는 핵심 진입점입니다. [Source/MyRatchet/Public/Characters/Enemy/BTTask_ActiveAbility.h](Source/MyRatchet/Public/Characters/Enemy/BTTask_ActiveAbility.h#L13-L27)

### Build 설정 및 태그 추가
- AI/BT 사용을 위해 AIModule을 의존성에 추가했습니다. [Source/MyRatchet/MyRatchet.Build.cs](Source/MyRatchet/MyRatchet.Build.cs#L11-L20)
- 근접 공격 태그 Enemy.Attack.Melee를 추가했습니다. [Config/Tags/Enemy.ini](Config/Tags/Enemy.ini#L1-L2)

## 콘텐츠 에셋 변경 (BB/BT/블루프린트)
- 신규/갱신된 에셋
  - Content/_M_RAC/Character/Enemy/BB_Enemy_Base.uasset: 적 AI 블랙보드 베이스.
  - Content/_M_RAC/Character/Enemy/BT_Enemy_Base.uasset: 적 AI 비헤이비어 트리 베이스.
  - Content/_M_RAC/Character/Enemy/BP_EnemyAIController.uasset: AEnemyAIController 기반 블루프린트.
  - Content/_M_RAC/Character/Enemy/BP_MeleeEnemy.uasset: 근접형 적 블루프린트.
- 정리된 에셋
  - Content/_M_RAC/Character/Enemy/BB_Enemy.uasset: 베이스 에셋으로 교체되며 제거.
  - Content/_M_RAC/Character/Enemy/BT_Enemy.uasset: 베이스 에셋으로 교체되며 제거.
- 맵/외부 액터 데이터 갱신
  - Content/__ExternalActors__/.../Basic_Map/...: AI 배치 및 테스트와 연관된 맵 외부 액터 데이터 업데이트.

## 동작 흐름 요약
1) 적 캐릭터 스폰 시 GAS 초기화 및 기본 어빌리티 부여
2) AI 컨트롤러가 Perception으로 플레이어 감지
3) BB에 타깃/시야 정보를 기록하고 BT가 상태 전환 및 행동 실행
4) BTTask에서 AbilityTag에 매핑된 능력 발동
