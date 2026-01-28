# Add RAC_Attribute + RAC_PlayerState & binding

## 개요
GAS(Gameplay Ability System)를 활용한 속성 관리 시스템을 구축하기 위해 `URAC_AttributeSet`과 `ARAC_PlayerState` 클래스를 생성하고, 이들을 캐릭터와 연결(binding)하는 아키텍처 구현입니다.

## 주요 변경 사항

### 1. 새로운 클래스 생성

#### `URAC_AttributeSet` (신규)
- **경로**: `Source/MyRatchet/Public/Characters/RAC_AttributeSet.h` / `.cpp`
- **역할**: GAS의 모든 게임 속성(Attributes) 정의 및 관리

**주요 기능:**
- GAS `AttributeSet`을 상속받아 구현
- 총 8개의 `FGameplayAttributeData` 속성 관리
- 매크로 기반 Getter/Setter 자동 생성

**정의된 속성:**

| 속성 | 타입 | 초기값 | 설명 |
|------|------|--------|------|
| `Health` | FGameplayAttributeData | 100.0f | 현재 체력 |
| `MaxHealth` | FGameplayAttributeData | 100.0f | 최대 체력 |
| `Bolts` | FGameplayAttributeData | 0.0f | 재화(볼트) |
| `Experience` | FGameplayAttributeData | 0.0f | 경험치 |
| `CurrentAmmo` | FGameplayAttributeData | 20.0f | 현재 탄약 |
| `MaxAmmo` | FGameplayAttributeData | 20.0f | 최대 탄약 |
| `DamageMultiplier` | FGameplayAttributeData | 1.0f | 데미지 배수 |
| `DashCooldown` | FGameplayAttributeData | 0.0f | 대시 쿨다운 |

**핵심 매크로:**
```cpp
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
```
- 각 속성별로 Getter, Setter, Initializer 함수 자동 생성
- 코드 중복 최소화 및 GAS 표준 패턴 준수

**주요 메서드:**
```cpp
URAC_AttributeSet();  // 생성자 - 모든 속성 초기화
virtual void PostGameplayEffectExecute(...);  // GameplayEffect 적용 후 콜백
```

**속성 값 클램핑:**
- Health는 PostGameplayEffectExecute에서 [0, MaxHealth] 범위로 클램핑
- 다른 속성들의 값 제한은 필요시 추가 가능

#### `ARAC_PlayerState` (신규)
- **경로**: `Source/MyRatchet/Public/Frameworks/RAC_PlayerState.h` / `.cpp`
- **역할**: Ability System을 관리하는 플레이어 상태 클래스

**주요 기능:**
- `IAbilitySystemInterface` 구현
- `UAbilitySystemComponent` 소유 및 초기화
- `URAC_AttributeSet` 생성 및 관리

**주요 멤버 변수:**
```cpp
UPROPERTY(VisibleAnywhere, Category="Abilities")
class UAbilitySystemComponent* AbilitySystemComponent;

UPROPERTY()
class URAC_AttributeSet* AttributeSet;
```

**초기화 과정 (생성자):**
1. `AbilitySystemComponent` 생성
2. `SetIsReplicated(true)` - 네트워크 복제 활성화
3. `URAC_AttributeSet` 생성

**주요 메서드:**
```cpp
virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
```
- `IAbilitySystemInterface` 구현
- 다른 시스템에서 PlayerState의 ASC에 접근할 수 있게 함

### 2. RAC_CPP_Character 클래스 수정

#### 추가된 기능
- **IAbilitySystemInterface 구현**
  ```cpp
  class MYRATCHET_API ARAC_CPP_Character : public ACharacter, public IAbilitySystemInterface
  ```
  - 캐릭터가 자신의 PlayerState를 통해 ASC에 접근하게 함

- **GetAbilitySystemComponent() 메서드 추가**
  ```cpp
  virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
  ```
  - PlayerState에서 ASC를 가져와 반환
  - PlayerState가 없으면 nullptr 반환

- **Binding 메서드 추가 (PossessedBy, OnRep_PlayerState)**
  ```cpp
  virtual void PossessedBy(AController* NewController) override;
  virtual void OnRep_PlayerState() override;
  ```
  - 서버 바인딩: `PossessedBy()`에서 `InitAbilityActorInfo()` 호출
  - 클라이언트 바인딩: `OnRep_PlayerState()`에서 `InitAbilityActorInfo()` 호출
  - ASC가 플레이어 캐릭터 및 PlayerState 정보로 초기화됨

- **BeginPlay()에서의 ASC 검증**
  ```cpp
  void ARAC_CPP_Character::BeginPlay()
  {
      Super::BeginPlay();
      
      if (GetAbilitySystemComponent())
      {
          UE_LOG(LogTemp, Warning, TEXT("GAS: Successfully linked to ASC on PlayerState!"));
      }
      else
      {
          UE_LOG(LogTemp, Error, TEXT("GAS: Failed to find ASC! Check your PlayerState or Casting."));
      }
  }
  ```

#### 포함된 헤더 파일
```cpp
#include "Frameworks/RAC_PlayerState.h"
#include "AbilitySystemComponent.h"
```

### 3. 아키텍처 변화

**GAS 시스템 바인딩 플로우:**

```
GameMode 생성 → PlayerState 생성 (ASC + AttributeSet)
   ↓
Controller가 Pawn Possess
   ↓
PossessedBy() 호출
   ↓
ASC->InitAbilityActorInfo(PlayerState, Character) 호출
   ↓
Character에서 GetAbilitySystemComponent() 통해 접근 가능
```

**계층 구조:**
```
APlayerController
    ↓ Possesses
ARAC_CPP_Character (implements IAbilitySystemInterface)
    ↓ GetPlayerState<>()
ARAC_PlayerState (implements IAbilitySystemInterface)
    ├─ UAbilitySystemComponent
    └─ URAC_AttributeSet
```

## 주요 특징

### 1. **GAS 표준 패턴 준수**
- `IAbilitySystemInterface` 구현으로 다양한 엔티티 지원
- `InitAbilityActorInfo()`를 통한 명시적 초기화

### 2. **서버-클라이언트 동기화**
- `AbilitySystemComponent::SetIsReplicated(true)`
- `PossessedBy()` (서버) 및 `OnRep_PlayerState()` (클라이언트) 바인딩

### 3. **속성 관리 중앙화**
- 모든 게임 속성이 AttributeSet에 정의
- 데이터 기반 속성 변경 (GameplayEffect 활용 가능)

### 4. **확장성**
- 새로운 속성 추가 시 AttributeSet에만 추가하면 됨
- GameplayEffect와 Ability로 속성 변경 로직 구현 가능

## 커밋 정보

### 커밋: `Add RAC_Attibute + RAC_PlayerState & binding`
```
GAS Attribute System 구축 및 PlayerState 연동
```
- `URAC_AttributeSet` 클래스 신규 생성
- `ARAC_PlayerState` 클래스 신규 생성
- `ARAC_CPP_Character`에 GAS 바인딩 로직 추가
- 총 X개 파일 변경

## 파일 변경 요약

| 파일 | 변경 유형 | 내용 |
|------|---------|------|
| `RAC_AttributeSet.h` | 신규 | AttributeSet 헤더 (8개 속성) |
| `RAC_AttributeSet.cpp` | 신규 | AttributeSet 구현 (초기화 및 값 클램핑) |
| `RAC_PlayerState.h` | 신규 | PlayerState 헤더 |
| `RAC_PlayerState.cpp` | 신규 | PlayerState 구현 (ASC/AttributeSet 생성) |
| `RAC_CPP_Character.h` | 수정 | IAbilitySystemInterface 상속, GetAbilitySystemComponent() 추가 |
| `RAC_CPP_Character.cpp` | 수정 | BeginPlay(), PossessedBy(), OnRep_PlayerState(), GetAbilitySystemComponent() 구현 |
| `RAC_Cpp_GameMode.h/.cpp` | 기존 유지 | PlayerState 클래스 자동 할당 설정 필요 (게임모드 설정) |

## 다음 단계

1. **GameMode 설정**
   - 프로젝트 설정에서 기본 PlayerState 클래스를 `ARAC_PlayerState`로 지정
   - 또는 GameMode의 `PlayerStateClass` 설정

2. **GameplayEffect 구현**
   - 속성 변경 로직을 GE(GameplayEffect)로 정의

3. **GameplayAbility 구현**
   - 실제 능력(공격, 회복 등)을 Ability로 구현

4. **UI 연동**
   - AttributeSet의 속성 값을 UI에 바인딩하여 표시

## 주의사항

- PlayerState가 제때 생성되지 않으면 ASC가 null이 될 수 있음
- 항상 `GetAbilitySystemComponent()` 반환 값이 valid한지 확인 필요
- 네트워크 게임에서는 `SetIsReplicated(true)` 필수
- `InitAbilityActorInfo()`는 반드시 PlayerState와 Character가 모두 준비된 후 호출

