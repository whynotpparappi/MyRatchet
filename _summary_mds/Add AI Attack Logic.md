# Add AI Attack Logic

## 개요
- 적 AI에 원거리 공격(GAS 기반) 흐름을 추가하고, 블랙보드에 근거리/원거리 사거리 및 범위 진입 상태를 기록하도록 확장.
- 비헤이비어 트리에서 태그 기반으로 능력을 시도하는 BT Task를 추가.
- AI 컨트롤러가 근거리/원거리 사거리를 블랙보드에 복사하도록 보강.
- 원거리 공격 태그를 Gameplay Tags에 등록.

## 변경된 코드/설정 파일과 역할

### Config/Tags/Enemy.ini
- Enemy.Attack.Ranged 태그를 추가.
- GAS 능력 태그 매칭에 사용되어 BT Task가 원거리 공격 능력을 발동할 수 있도록 함.

### Source/MyRatchet/Public/Abilities/GA_EnemyRangedAttack.h
- 원거리 공격 Gameplay Ability의 기본 데이터와 인터페이스 정의.
- 주요 속성
  - ProjectileClass: 발사체 클래스.
  - ProjectileSpeed: 발사체 속도.
  - MuzzleSocketName / WeaponMeshComponentName: 발사 위치를 찾기 위한 소켓/컴포넌트 설정.
  - TargetActorKey / TargetLocationZOffset: 블랙보드 타겟 참조 및 조준 위치 보정.
- 주요 함수
  - ActivateAbility(): 발사 로직 진입점.
  - GetTargetActorFromBlackboard(): 블랙보드에서 타겟 액터 추출.
  - GetMuzzleTransform(): 무기 메시에 있는 소켓 트랜스폼을 확보(StaticMesh 우선, SkeletalMesh 백업).

### Source/MyRatchet/Private/Abilities/GA_EnemyRangedAttack.cpp
- ActivateAbility()에서 발사체 스폰 및 초기 속도 설정까지 수행.
- 블랙보드 타겟이 없거나 월드/메쉬/클래스가 없으면 안전하게 종료.
- 타겟 위치를 Z 오프셋으로 보정하고, 발사 방향을 계산해 발사체를 스폰.

### Source/MyRatchet/Public/Characters/Enemy/BTS_UpdateCombatData.h
- 블랙보드 키 셋 확장
  - AttackRangeKey, RangedAttackRangeKey: 근거리/원거리 사거리.
  - IsInAttackRangeKey, IsInRangedRangeKey: 사거리 진입 상태.
- 히스테리시스 버퍼
  - MeleeExitBuffer, RangedExitBuffer로 경계 깜빡임을 완화.

### Source/MyRatchet/Private/Characters/Enemy/BTS_UpdateCombatData.cpp
- TickNode()에서 타겟 거리, LOS, 근거리/원거리 진입 상태를 업데이트.
- 타겟이 없을 때 LOS/범위 플래그를 안전하게 해제.
- 근거리 범위와 원거리 범위가 겹치지 않도록 원거리 판단에 Distance > MeleeRange 조건을 추가.

### Source/MyRatchet/Public/Characters/Enemy/BTTask_TryActivateAbility.h
- 비헤이비어 트리에서 GAS 능력을 태그로 시도하는 Task 정의.
- AbilityTag: 발동할 능력 태그.
- bSucceedWhenNotActivated: 실패해도 트리를 끊지 않고 성공 처리할지 옵션.

### Source/MyRatchet/Private/Characters/Enemy/BTTask_TryActivateAbility.cpp
- ExecuteTask()에서 AI Pawn의 ASC를 찾아 AbilityTag로 TryActivateAbilitiesByTag 수행.
- 활성화 실패 시에도 옵션에 따라 성공/실패 반환.

### Source/MyRatchet/Public/Characters/Enemy/EnemyAIController.h
- RangedAttackRangeKey를 추가하여 블랙보드에서 원거리 사거리 키를 관리.

### Source/MyRatchet/Private/Characters/Enemy/EnemyAIController.cpp
- OnPossess()에서 EnemyCharacterBase의 원거리 사거리를 블랙보드에 복사.

### Source/MyRatchet/Public/Characters/Enemy/EnemyCharacterBase.h
- 원거리 사거리 속성 RangedAttackRange 추가.
- GetRangedAttackRange()를 통해 AI 컨트롤러가 값을 조회할 수 있게 제공.
- 기존 AttackRange는 근거리 사거리로 의미를 명확화.

## 에디터/콘텐츠 변경 요약
- 비헤이비어 트리 및 블랙보드 관련 에셋, 적 캐릭터 BP에 원거리 공격 흐름이 연결됨.
- 원거리 공격 능력용 BP 에셋이 추가되어 GAS-태그 기반 발동에 대응.
