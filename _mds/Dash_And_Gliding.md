# Dash & Gliding 기능 추가

**커밋**: `66ea26e` - add Dash & Gliding  
**작성자**: kim sa yun  
**날짜**: 2026-01-23

---

## 📋 변경 사항 요약

이번 커밋은 대시(Dash) 기능의 기존 구현을 유지하면서 **글라이딩(Gliding) 기능**을 새로 추가했습니다.

---

## 🎮 주요 기능

### 1. Dash (기존 기능 유지)
- 대시 거리, 속도, 쿨타임 설정 가능
- 대시 중 무적 상태 구현
- 입력 방향 또는 전진 방향으로 대시
- 기존 구현 유지하되 일부 코드 정리

### 2. Gliding (새로 추가)
- **점프 버튼 长按 메커니즘**: Jump 버튼을 일정 시간(0.25초) 이상 누르고 있으면 글라이딩 활성화
- **글라이딩 조건**:
  - 공중에 있어야 함 (Falling 상태)
  - 수직 속도가 음수(하강 중)여야 함
  - 점프 버튼 지속 누르고 있어야 함
- **중력 감소**: 글라이딩 중 `GravityScale` 값을 0.2로 감소시켜 낙하 속도 제어
- **착지 시 자동 해제**: 착지하면 자동으로 글라이딩 종료

---

## 🔧 코드 변경 상세

### Header 파일 (`RAC_CPP_Character.h`)

#### 추가된 변수:
```cpp
// Gliding 관련 변수
UPROPERTY(editAnywhere, BlueprintReadOnly, Category="Glide")
bool bJumpHeld = false;  // 점프 버튼 누르고 있는 상태

UPROPERTY(editAnywhere, BlueprintReadOnly, Category="Glide")
bool bGlideRequested = false;  // 글라이딩 요청 상태

UPROPERTY(editAnywhere, BlueprintReadOnly, Category="Glide")
bool bIsGliding = false;  // 현재 글라이딩 중 여부

// 글라이딩 설정 값
FTimerHandle GlideHoldTimerHandle;
float GlideHoldThreshold = 0.25f;  // 점프 버튼 누르고 있어야 하는 시간
float GlideGravityScale = 0.3f;    // 글라이딩 중 중력 스케일
float GlideFallSpeed = -300.0f;    // 글라이딩 최대 하강 속도
float DefaultGravityScale = 1.5f;  // 기본 중력 스케일
float GlideThreshold = 0.3f;       // 글라이딩 시작 임계값
float GlidingGravityScale = 0.2f;  // 글라이딩 중력 스케일
```

#### 추가된 함수:
```cpp
void OnJumpStarted();              // 점프 시작 시 호출
void OnJumpCompleted();            // 점프 종료 시 호출
void TryStartGlideFromHold();      // 버튼 누르고 있는 시간 초과 시 글라이딩 시도
void OnMovementModeChanged(...);   // 이동 모드 변경 시 호출 (Override)
void StartGlide();                 // 글라이딩 시작
void StopGlide();                  // 글라이딩 종료
void Landed(...);                  // 착지 시 호출 (Override)
```

### Implementation 파일 (`RAC_CPP_Character.cpp`)

#### 생성자 변경:
```cpp
JumpMaxCount = 2;  // 이중 점프 가능하도록 설정
GetCharacterMovement()->MaxJumpApexAttemptsPerSimulation = 2;  // 제거됨
```

#### BeginPlay 변경:
```cpp
DefaultGravityScale = GetCharacterMovement()->GravityScale;  // 기본 중력값 저장
```

#### Tick 함수 변경:
- 글라이딩 조건 확인 로직 추가
- 점프 중 글라이딩 요청 시 자동 시작

#### SetupPlayerInputComponent 변경:
```cpp
// 기존: Jump / StopJumping
// 변경: OnJumpStarted / OnJumpCompleted
EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, 
    this, &ARAC_CPP_Character::OnJumpStarted);
EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, 
    this, &ARAC_CPP_Character::OnJumpCompleted);
```

#### 글라이딩 함수 구현:
```cpp
OnJumpStarted()
  ├─ bJumpHeld = true
  ├─ 0.25초 타이머 시작 (TryStartGlideFromHold 호출)
  └─ Jump() 실행

OnJumpCompleted()
  ├─ bJumpHeld = false
  ├─ 타이머 해제
  └─ 글라이딩 중이면 StopGlide() 호출

TryStartGlideFromHold()
  └─ bGlideRequested = true (버튼 누르고 있으면)

StartGlide()
  ├─ 검증: 이미 글라이딩 중이거나 Falling 상태 아니면 반환
  ├─ bIsGliding = true
  └─ GravityScale = GlideGravityScale (0.2)

StopGlide()
  ├─ bIsGliding = false
  └─ GravityScale = DefaultGravityScale 복원

Landed()
  ├─ Super::Landed() 호출
  └─ StopGlide()
```

---

## 📦 수정된 에셋

| 에셋 | 변경 사항 |
|------|---------|
| `BP_ThirdPersonCharacter.uasset` | 블루프린트 업데이트 |
| `ABP_RAC_Wraith_AnimBlueprint1.uasset` | 애니메이션 블렌드스페이스 수정 |
| `Bp_RAC_CPP_Character.uasset` | 캐릭터 블루프린트 확장 |
| `RAC_PlayerCharacter.uasset` | 플레이어 캐릭터 설정 변경 |
| `Move_Mapping.uasset` | 입력 매핑 추가 |
| `IA_R_Jump.uasset` | 점프 입력 액션 수정 |
| `BP_splines.uasset` | 새로운 스플라인 액터 추가 |

---

## 🐛 디버그 로그

다음 로그를 통해 글라이딩 상태를 모니터링할 수 있습니다:

```
[TryStartGlideFromHold] HoldReached bJumpHeld=1 Falling=1 VelZ=-300.00
[OnMovementModeChanged] ModeChanged NowFalling=1 Requested=1 Held=1 VelZ=-300.00
[StartGlide] GlideStart
```

---

## 📝 주석 코드

기능 구현 중 검토 중인 부분들이 주석 처리되어 있습니다:

- **After Image 효과** (향후 구현 예정)
  - `bUseAfterImage`, `AfterImageInterval`, `AfterImageLifeTime`, `AfterImageMaterial` 등 변수 정의만 주석 처리

---

## 🔄 점프 메커니즘 변경 사항

| 기능 | 이전 | 현재 |
|------|------|------|
| 점프 입력 | `ACharacter::Jump` 직접 호출 | `OnJumpStarted` 호출 |
| 점프 종료 | `ACharacter::StopJumping` 직접 호출 | `OnJumpCompleted` 호출 |
| 이중 점프 | `MaxJumpApexAttemptsPerSimulation = 2` | `JumpMaxCount = 2` |

---

## ✅ 테스트 항목

- [x] 대시 기능 정상 작동
- [x] 글라이딩 버튼 홀드 감지
- [x] 글라이딩 조건 확인 (공중, 하강)
- [x] 글라이딩 중 중력 감소
- [x] 착지 시 글라이딩 자동 종료
- [ ] 점프 + 글라이딩 조합 동작
- [ ] 이중 점프 + 글라이딩 조합
