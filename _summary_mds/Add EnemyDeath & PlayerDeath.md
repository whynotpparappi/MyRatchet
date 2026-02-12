# Add EnemyDeath & PlayerDeath

## 변경 요약
- 적/플레이어 사망 처리 플로우를 추가하고, 체력 변화에 맞춰 AI/입력/이펙트/UI가 정지되도록 정리.
- 발사체에 데미지 전달과 충돌 처리 로직을 추가해 피격 시 체력 감소가 일관되게 적용.
- HUD가 ASC의 체력 변화 델리게이트를 통해 즉시 갱신되도록 개선.

## 파일별 변경 및 함수 역할

### [Source/MyRatchet/Public/Characters/Enemy/EnemyCharacterBase.h](Source/MyRatchet/Public/Characters/Enemy/EnemyCharacterBase.h)
- `AEnemyCharacterBase::TakeDamage()` 선언 추가로 적이 직접 데미지를 처리하는 진입점 제공.
- `KillEnemy()` 블루프린트 호출로 즉시 사망시키는 유틸리티 제공.
- `MaxHealth`, `CurrentHealth`, `bIsDead`, `DeathMontage`, `DeathDespawnDelay`를 도입해 사망 연출과 소멸 타이밍 제어.
- `HandleHealthChanged()`, `HandleDeath()`, `OnDeathMontageEnded()`로 체력 변화 및 사망 상태 전이를 분리.

### [Source/MyRatchet/Private/Characters/Enemy/EnemyCharacterBase.cpp](Source/MyRatchet/Private/Characters/Enemy/EnemyCharacterBase.cpp)
- `InitializeAbilitySystem()`에서 `MaxHealth`를 ASC에 주입하고 체력 변화 델리게이트를 바인딩.
- `TakeDamage()`에서 데미지를 ASC의 `Health`에 적용하고 0 이하 시 `HandleDeath()`로 전환.
- `KillEnemy()`는 사망 플래그 체크 후 체력을 0으로 만들고 `HandleDeath()` 호출.
- `HandleHealthChanged()`는 ASC 체력 변화 콜백으로 사망 조건을 감지.
- `HandleDeath()`는 능력 취소, 이동/충돌 비활성화, AI 정지 및 사망 몽타주 재생 후 소멸을 처리.
- `OnDeathMontageEnded()`는 몽타주 종료 시 수명 타이머를 설정.

### [Source/MyRatchet/Public/Characters/Enemy/EnemyAIController.h](Source/MyRatchet/Public/Characters/Enemy/EnemyAIController.h)
- `HandleControlledPawnDeath()` 선언으로 AI가 사망 상태를 블랙보드에 반영할 수 있게 함.
- `IsDead`, `ActionState` 블랙보드 키를 추가해 사망/상태 전환에 활용.

### [Source/MyRatchet/Private/Characters/Enemy/EnemyAIController.cpp](Source/MyRatchet/Private/Characters/Enemy/EnemyAIController.cpp)
- `OnPossess()`에서 `IsDead=false`, `ActionState=Idle` 초기화.
- `OnTargetDetected()`에서 사망 상태면 감지 로직을 무시.
- `HandleControlledPawnDeath()`가 블랙보드 키를 갱신하고 이동을 멈추며 타겟을 제거.

### [Source/MyRatchet/Private/Characters/Enemy/BTS_UpdateCombatData.cpp](Source/MyRatchet/Private/Characters/Enemy/BTS_UpdateCombatData.cpp)
- `TickNode()`에서 적이 사망 상태면 시야/사거리 관련 키를 false로 만들고 즉시 종료.

### [Source/MyRatchet/Public/Characters/RAC_CPP_Character.h](Source/MyRatchet/Public/Characters/RAC_CPP_Character.h)
- `TakeDamage()` 선언으로 플레이어도 직접 데미지 처리.
- `BindHealthChangedDelegate()`, `OnHealthAttributeChanged()`, `HandleDeath()`, `OnDeathMontageEnded()`로 사망 플로우 구성.
- `bIsDead`, `DeathMontage`, `DeathDespawnDelay`, `HealthChangeDelegateHandle` 추가로 사망 상태와 델리게이트 관리.

### [Source/MyRatchet/Private/Characters/RAC_CPP_Character.cpp](Source/MyRatchet/Private/Characters/RAC_CPP_Character.cpp)
- `TakeDamage()`에서 ASC 체력 감소 및 0 이하 시 `HandleDeath()` 실행.
- `PossessedBy()` / `OnRep_PlayerState()`에서 체력 변화 델리게이트 바인딩.
- `BindHealthChangedDelegate()`는 ASC 체력 변화 이벤트를 등록.
- `OnHealthAttributeChanged()`는 체력 0 이하를 감지해 사망 처리.
- `HandleDeath()`는 입력/상태 리셋, 능력 취소, 이동/충돌 비활성화, 사망 몽타주 처리.
- `OnDeathMontageEnded()`는 몽타주 종료 시 소멸 타이머 설정.
- `HandleShoot()`가 `bIsDead` 체크로 사망 시 사격을 차단.

### [Source/MyRatchet/Public/Combat/RAC_ProjectileBase.h](Source/MyRatchet/Public/Combat/RAC_ProjectileBase.h)
- `ARAC_ProjectileBase` 신규 추가로 공통 발사체 베이스 클래스 정의.
- `Damage`, `DamageTypeClass`, 충돌/이동 컴포넌트를 노출해 데미지 전달을 중앙화.

### [Source/MyRatchet/Private/Combat/RAC_ProjectileBase.cpp](Source/MyRatchet/Private/Combat/RAC_ProjectileBase.cpp)
- `BeginPlay()`에서 충돌/오버랩 이벤트 바인딩 및 자기 충돌 방지 설정.
- `HandleImpact()`가 아군끼리의 피격을 무시하고, ASC가 있으면 직접 `Health`를 감소.
- ASC가 없는 대상은 `ApplyDamage()`로 일반 데미지를 적용 후 발사체 제거.
- 발사체끼리 충돌 시 서로 제거하는 처리 포함.

### [Source/MyRatchet/Public/Abilities/GA_AutoFire.h](Source/MyRatchet/Public/Abilities/GA_AutoFire.h)
- `ProjectileDamage`, `SpawnForwardOffset` 추가로 데미지 값과 스폰 오프셋을 노출.

### [Source/MyRatchet/Private/Abilities/GA_AutoFire.cpp](Source/MyRatchet/Private/Abilities/GA_AutoFire.cpp)
- `SpawnForwardOffset` 적용으로 총구 앞에서 스폰되도록 개선.
- 발사체 루트 컴포넌트에 자기 충돌 무시 설정.
- `ARAC_ProjectileBase`일 경우 `Damage` 값을 전달.

### [Source/MyRatchet/Public/Abilities/GA_EnemyRangedAttack.h](Source/MyRatchet/Public/Abilities/GA_EnemyRangedAttack.h)
- `ProjectileDamage`, `SpawnForwardOffset` 추가로 원거리 적 투사체 데미지를 노출.

### [Source/MyRatchet/Private/Abilities/GA_EnemyRangedAttack.cpp](Source/MyRatchet/Private/Abilities/GA_EnemyRangedAttack.cpp)
- 발사체 스폰 위치를 전방 오프셋으로 보정.
- 자기 충돌 무시 처리와 `ARAC_ProjectileBase::Damage` 전달 추가.

### [Source/MyRatchet/Public/Widgets/RAC_HUD_Widget.h](Source/MyRatchet/Public/Widgets/RAC_HUD_Widget.h)
- ASC 체력 변화 델리게이트를 위한 `BindToAbilitySystem()` 및 핸들 보관 필드 추가.

### [Source/MyRatchet/Private/Widgets/RAC_HUD_Widget.cpp](Source/MyRatchet/Private/Widgets/RAC_HUD_Widget.cpp)
- `BindToAbilitySystem()`에서 ASC의 체력 변화 델리게이트를 구독.
- `OnHealthAttributeChanged()`가 체력 UI를 즉시 갱신.
- `NativeDestruct()`에서 델리게이트 해제를 보장.

### 문서
- [MYRatchet/Abilities/GA_AutoFire.md](MYRatchet/Abilities/GA_AutoFire.md) 신규 생성: 책임/의존성/관련 클래스 요약.
- [MYRatchet/Abilities/GA_EnemyRangedAttack.md](MYRatchet/Abilities/GA_EnemyRangedAttack.md) 신규 생성 (내용 없음).
- [MYRatchet/EnemyAIController.md](MYRatchet/EnemyAIController.md) 신규 생성 (내용 없음).
