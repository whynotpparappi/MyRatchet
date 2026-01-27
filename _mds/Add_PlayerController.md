# Add PlayerController + Edit Player

## 개요
플레이어 입력 처리를 분리하기 위해 전용 `PlayerController` 클래스를 생성하고, 기존의 `Character` 클래스에서 입력 처리 로직을 제거한 리팩토링 작업입니다.

## 주요 변경 사항

### 1. 새로운 클래스 생성

#### `ARAC_PlayerController` (신규)
- **경로**: `Source/MyRatchet/Public/Frameworks/RAC_PlayerController.h` / `.cpp`
- **역할**: 모든 플레이어 입력 처리 담당

**주요 기능:**
- Enhanced Input System을 이용한 입력 관리
- 입력 이벤트를 Character에 전달하는 중개자 역할

**주요 메서드:**
```cpp
virtual void BeginPlay() override;           // 입력 매핑 컨텍스트 추가
virtual void SetupInputComponent() override; // 입력 액션 바인딩

// 입력 콜백 함수들
void OnMove(const FInputActionValue& Value);
void OnLook(const FInputActionValue& Value);
void OnJumpStarted();
void OnJumpCompleted();
void OnAim(const FInputActionValue& Value);
void OnShoot(const FInputActionValue& Value);
void OnDash(const FInputActionValue& Value);
void OnMelee(const FInputActionValue& Value);
```

**관리하는 입력 액션:**
- `MoveAction`: 이동
- `LookAction`: 카메라 회전
- `JumpAction`: 점프
- `AimAction`: 조준
- `DashAction`: 대시
- `ShootAction`: 발사
- `MeleeAction`: 근접 공격
- `CharacterMappingContext`: 입력 매핑 컨텍스트

### 2. RAC_CPP_Character 클래스 수정

#### 제거된 항목
- **Input 관련 UPROPERTY 제거**:
  - `CharacterMappingContext`
  - `MoveAction`, `LookAction`, `JumpAction`, `AimAction`, `DashAction`, `ShootAction`, `MeleeAction`

- **SetupPlayerInputComponent() 함수 제거**:
  - 입력 매핑 컨텍스트 추가 로직 제거
  - 입력 액션 바인딩 로직 제거
  - 대신 `Super::SetupPlayerInputComponent(PlayerInputComponent)` 호출로 축약

#### 수정된 항목
- **Jump 메서드명 변경**:
  - `OnJumpStarted()` → `JumpStarted()` (public 메서드)
  - `OnJumpCompleted()` → `JumpCompleted()` (public 메서드)

- **Glide 관련 타입 수정**:
  - `UPROPERTY(editAnywhere, ...)` → `UPROPERTY(EditAnywhere, ...)` (대문자 E 통일)

- **메서드 접근성 변경**:
  - `Move()`, `Look()`, `Aim()`, `Shoot()`, `Dash()`, `Melee()` 메서드를 `protected`에서 `public`으로 변경
  - `JumpStarted()`, `JumpCompleted()`, `TryStartGlideFromHold()`도 `protected`에서 `public`으로 변경

#### 추가된 메서드
```cpp
void Landed(const FHitResult& Hit) override;
void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;
```

### 3. 아키텍처 변화

**이전 (Character 중심):**
```
Player Input → Character → Input Setup & Callback Functions
```

**이후 (Controller 분리):**
```
Player Input → PlayerController → Input Setup & Callback → Character (Input 처리)
```

## 커밋 정보

### 커밋 1: `073e10e` (2026-01-27 13:34:24)
```
Add PlayerController + Edit Player
컨트롤러 분리
```
- `ARAC_PlayerController` 클래스 신규 생성 (54줄 헤더, 110줄 구현)
- Character 클래스에서 입력 처리 로직 제거 및 공개화
- 총 38개 파일 변경 (549 insertions, 245 deletions)

### 커밋 2: `6cc3cfa` (2026-01-27 13:39:31)
```
Add PlayerController + Edit Playercharacter
```
- Character의 `SetupPlayerInputComponent()` 함수 완전 제거
- 대신 부모 클래스의 구현으로 위임
- 3개 파일 변경 (3 insertions, 53 deletions)

## 장점

1. **관심사 분리 (Separation of Concerns)**
   - Input 처리는 PlayerController에서, Character는 움직임에만 집중

2. **코드 재사용성 증대**
   - Character 클래스를 다른 컨트롤러와도 쉽게 연계 가능

3. **유지보수성 개선**
   - 입력 처리 로직이 한 곳에 집중되어 관리 용이

4. **확장성 증대**
   - AI 컨트롤러나 다른 입력 스키마 추가 시 유연함

## 파일 변경 요약

| 파일 | 변경 유형 | 내용 |
|------|---------|------|
| `RAC_PlayerController.h` | 신규 | PlayerController 헤더 (54줄) |
| `RAC_PlayerController.cpp` | 신규 | PlayerController 구현 (110줄) |
| `RAC_CPP_Character.h` | 수정 | Input 관련 UPROPERTY 제거, 메서드 접근성 변경 |
| `RAC_CPP_Character.cpp` | 수정 | SetupPlayerInputComponent 제거, Jump 메서드명 변경 |
| `RAC_GameInstance.h/.cpp` | 수정 | 코드 추가 (정확한 내용은 diff 확인 필요) |
| `RAC_GameModeBase.h/.cpp` | 수정 | 코드 추가 (정확한 내용은 diff 확인 필요) |
| `RAC_GameState.h/.cpp` | 수정 | 코드 추가 (정확한 내용은 diff 확인 필요) |
| 각종 에셋 파일 | 수정/신규 | 블루프린트, 애니메이션, 위젯 등 |

## 이동된 메서드

다음 메서드들은 이제 PlayerController에서 호출되며, Character는 이들을 구현만 담당합니다:

| 메서드 | PlayerController | Character |
|--------|-----------------|-----------|
| `OnMove()` | ✓ (입력 수신) | `Move()` 호출 |
| `OnLook()` | ✓ (입력 수신) | `Look()` 호출 |
| `OnJumpStarted()` | ✓ (입력 수신) | `JumpStarted()` 호출 |
| `OnJumpCompleted()` | ✓ (입력 수신) | `JumpCompleted()` 호출 |
| `OnAim()` | ✓ (입력 수신) | `Aim()` 호출 |
| `OnShoot()` | ✓ (입력 수신) | `Shoot()` 호출 |
| `OnDash()` | ✓ (입력 수신) | `Dash()` 호출 |
| `OnMelee()` | ✓ (입력 수신) | `Melee()` 호출 |
