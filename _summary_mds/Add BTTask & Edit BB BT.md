# Add BTTask & Edit BB BT

## 개요
Enemy AI의 Behavior Tree/Blackboard 에셋을 업데이트한 커밋. BTTask 추가에 맞춰 Blackboard 스키마와 BT 흐름을 조정한 변경으로 보이며, 변경 대상은 에디터용 바이너리 에셋(blueprint uasset)이다.

## 주요 변경 사항 (상세)
- **BTTask 추가 (GAS 연동)**: [Source/MyRatchet/Public/Characters/Enemy/BTTask_ActiveAbility.h](Source/MyRatchet/Public/Characters/Enemy/BTTask_ActiveAbility.h) / [Source/MyRatchet/Private/Characters/Enemy/BTTask_ActiveAbility.cpp](Source/MyRatchet/Private/Characters/Enemy/BTTask_ActiveAbility.cpp)
  - 행동 트리에서 특정 어빌리티 태그를 기반으로 능력을 활성화하는 BTTask.
  - `AbilityTag`로 어떤 공격/스킬을 실행할지 에디터에서 선택 가능.
  - `ExecuteTask()`에서 `IAbilitySystemInterface`와 `UAbilitySystemComponent`를 통해 능력 실행을 시도.

- **Blackboard 업데이트**: [Content/_M_RAC/Character/Enemy/BB_Enemy.uasset](Content/_M_RAC/Character/Enemy/BB_Enemy.uasset)
  - Enemy AI가 사용하는 블랙보드 키 스키마를 갱신.
  - BTTask/Decorator/Service가 참조하는 상태 데이터를 통합 관리.
  - 역할(개념적):
    - 타깃/위치/상태 등 AI 의사결정에 필요한 런타임 값을 저장.
    - BT 노드 간 공유 데이터의 단일 소스 역할.
  - 참고: uasset 특성상 키 목록/타입/기본값은 에디터에서 확인 필요.

- **Behavior Tree 업데이트**: [Content/_M_RAC/Character/Enemy/BT_Enemy.uasset](Content/_M_RAC/Character/Enemy/BT_Enemy.uasset)
  - BTTask 추가 및 BT 흐름 수정.
  - 역할(개념적):
    - Enemy AI의 상위 의사결정 흐름(Selector/Sequence) 구성.
    - Blackboard 값에 따라 이동/탐색/공격 등 행동을 분기.
    - Decorator/Service를 통해 지속적인 상태 평가 및 갱신.
  - 참고: uasset 내부 노드 구성(정확한 Task/Decorator/Service 목록)은 에디터에서 확인 필요.

## 파일/함수(역할) 정리
- **BTTask_ActiveAbility.h**
  - `UBTTask_ActiveAbility`: GAS 능력 실행을 위한 BTTask 클래스 선언.
  - `AbilityTag`: 실행할 능력 태그를 지정하는 에디터 노출 프로퍼티.

- **BTTask_ActiveAbility.cpp**
  - `UBTTask_ActiveAbility()`: 에디터에서 보이는 노드 이름을 `Activate Ability`로 지정.
  - `ExecuteTask()`:
    - AI 컨트롤러/폰을 획득.
    - `IAbilitySystemInterface`로 ASC 접근.
    - `TryActivateAbilitiesByTag()`로 지정 태그 능력을 실행하고 성공/실패 반환.

- **BB_Enemy.uasset**
  - 함수 대신 블랙보드 키 정의가 핵심.
  - BTTask와 조건 노드가 참조하는 공통 상태 저장소 역할.

- **BT_Enemy.uasset**
  - 함수 대신 BT 노드(Selector/Sequence/Task/Decorator/Service)로 동작.
  - 전체 AI 행동 루프의 실행 순서와 조건을 정의.

## 메모
- 커밋 메시지의 "Add BTTask"는 BT_Enemy 내부 노드 추가 또는 기존 BTTask 참조 추가를 의미하는 것으로 보인다.
- 상세 노드/키는 바이너리 에셋 특성상 텍스트 추출이 어려우므로, Unreal Editor에서 확인 및 스크린샷/텍스트 덤프를 기반으로 추가 정리가 가능하다.
