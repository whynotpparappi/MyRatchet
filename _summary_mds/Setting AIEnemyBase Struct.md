# Setting AIEnemyBase Struct

## 개요
AI Enemy Base 구조(공용 베이스 + 타입 분기)를 정리하고, GAS/AI 컨트롤러 기반으로 적 캐릭터 프레임워크를 추가한 커밋. 조준 트레이스 채널을 전용 채널로 분리하고 디버그 시각화도 보강.

## 주요 변경 사항 (상세)
- **Enemy 베이스 프레임워크 신설**
  - `AEnemyCharacterBase` 신규 추가
    - `UEnemyAbilitySystemComponentBase` 서브오브젝트 생성 및 복제 설정(`Minimal`).
    - `AEnemyAIController` 자동 적용 및 `PlacedInWorldOrSpawned` 자동 Possess.
    - `InitializeAbilitySystem()`에서 `InitAbilityActorInfo(this, this)` 수행.
    - `EEnemyType` 열거형 도입(근거리/원거리/보스).
  - 파생 클래스 추가 및 타입 지정
    - `AEnemyMeleeCharacter` → `EEnemyType::Melee`
    - `AEnemyRangedCharacter` → `EEnemyType::Ranged`
    - `AEnemyBossCharacter` → `EEnemyType::Boss`
  - 베이스 컴포넌트/컨트롤러
    - `UEnemyAbilitySystemComponentBase` (GAS 전용 ASC 베이스)
    - `AEnemyAIController` (AI 컨트롤러 베이스)

- **조준 트레이스 채널 분리**
  - `DefaultEngine.ini`에 `Fire` 트레이스 채널 추가 및 프로필 응답 갱신.
  - `MyRatchet.h`에 `RatchetCollisionChannels::FireChannel` 상수 추가.
  - `GA_AutoFire`
    - `AimTraceChannel` 프로퍼티 추가(기본값: `FireChannel`).
    - `GetAimTargetPoint()` 라인트레이스를 `AimTraceChannel`로 전환.
    - 조준 레이 디버그 라인/포인트 표시 추가.

## 추가/수정 에셋(요약)
- 캐릭터/맵/UI 블루프린트 및 레벨 외부 액터 업데이트.
- ArrowTrail 데모용 BP/메시 에셋 추가.

## 메모
- 본 커밋은 AI Enemy Base 구조 확립을 위한 C++ 베이스 클래스 도입이 핵심이며, 블루프린트에서 해당 베이스 구조를 참조/배치하는 변경이 동반됨.
