# MyRatchet 프로젝트 - GAS(Gameplay Ability System) & ASC(Ability System Component) 완벽 가이드

## 📋 목차
1. [개요](#개요)
2. [GAS와 ASC의 개념](#gas와-asc의-개념)
3. [현재 구현 상황](#현재-구현-상황)
4. [시스템 아키텍처](#시스템-아키텍처)
5. [주요 클래스 설명](#주요-클래스-설명)
6. [사용 방법](#사용-방법)
7. [수정 및 확장 방법](#수정-및-확장-방법)
8. [주의사항](#주의사항)

---

## 개요

이 문서는 MyRatchet 프로젝트에 구현된 **GAS(Gameplay Ability System)**와 **ASC(Ability System Component)** 시스템에 대한 완벽한 가이드입니다.

**목표:**
- 플레이어 캐릭터의 모든 게임 속성(체력, 탄약, 경험치 등)을 중앙화하여 관리
- GameplayEffect와 GameplayAbility를 통한 데이터 기반 능력 시스템 구축
- 네트워크 멀티플레이 지원 준비

---

## GAS와 ASC의 개념

### GAS(Gameplay Ability System)란?
**언리얼 엔진의 강력한 플러그인**으로, 게임 캐릭터의 능력(Ability)과 속성(Attribute)을 체계적으로 관리합니다.

**주요 구성 요소:**

| 요소 | 설명 |
|------|------|
| **AttributeSet** | 캐릭터의 모든 게임 속성(체력, 마나, 공격력 등) 정의 |
| **AbilitySystemComponent (ASC)** | AttributeSet과 GameplayAbility를 관리하는 핵심 컴포넌트 |
| **GameplayEffect (GE)** | 속성값을 변경하는 효과 (버프, 디버프, 데미지 등) |
| **GameplayAbility (GA)** | 플레이어가 사용할 수 있는 능력 (공격, 방어, 회복 등) |

### ASC(Ability System Component)란?
**AttributeSet과 GameplayAbility를 관리하는 액터 컴포넌트**입니다.

**주요 역할:**
- AttributeSet과 GameplayAbility를 소유
- GameplayEffect 적용
- 능력 활성화 및 제어
- 속성값 변경 및 알림

---

## 현재 구현 상황

### ✅ 완료된 구현

#### 1. **URAC_AttributeSet** (캐릭터의 모든 속성 정의)
**파일:** [Characters/RAC_AttributeSet.h](Source/MyRatchet/Public/Characters/RAC_AttributeSet.h)

**현재 관리하는 15개 속성:**

```cpp
// 1. 기본 능력치 (Base Stats)
FGameplayAttributeData Health;           // 현재 체력 (초기값: 100)
FGameplayAttributeData MaxHealth;        // 최대 체력 (초기값: 100)
FGameplayAttributeData Experience;       // 경험치 (초기값: 0)
FGameplayAttributeData Level;            // 레벨 (초기값: 1)
FGameplayAttributeData DamageMultiplier; // 데미지 배수 (초기값: 1.0)

// 2. 경제 시스템 (Economy)
FGameplayAttributeData Bolts;            // 재화-볼트 (초기값: 0)
FGameplayAttributeData Raritanium;       // 재화-레어타늄 (초기값: 0)

// 3. 탄약 창고 (Ammo Reserve)
  // 3.1 기본 연사형 탄약 (Blaster)
  FGameplayAttributeData Ammo_Blaster;      // 현재 블래스터 탄약 (초기값: 0)
  FGameplayAttributeData MaxAmmo_Blaster;   // 최대 블래스터 탄약 (초기값: 6)
  
  // 3.2 투척/폭발형 탄약 (Bomb)
  FGameplayAttributeData Ammo_Bomb;         // 현재 폭탄 탄약 (초기값: 0)
  FGameplayAttributeData MaxAmmo_Bomb;      // 최대 폭탄 탄약 (초기값: 150)
  
  // 3.3 특수 에너지형 탄약 (Tesla)
  FGameplayAttributeData Ammo_Tesla;        // 현재 테슬라 탄약 (초기값: 0)
  FGameplayAttributeData MaxAmmo_Tesla;     // 최대 테슬라 탄약 (초기값: 150)

// 4. 유틸리티 (Utility)
FGameplayAttributeData DashCooldown;     // 대시 쿨다운 (초기값: 0)
```

| 카테고리 | 속성 | 초기값 | 설명 |
|---------|------|--------|------|
| **Base Stats** | `Health` | 100.0f | 현재 체력 |
| | `MaxHealth` | 100.0f | 최대 체력 |
| | `Experience` | 0.0f | 경험치 |
| | `Level` | 1 | 레벨 |
| | `DamageMultiplier` | 1.0f | 데미지 배수 |
| **Economy** | `Bolts` | 0 | 재화(볼트) |
| | `Raritanium` | 0 | 재화(레어타늄) |
| **Ammo-Blaster** | `Ammo_Blaster` | 0 | 현재 블래스터 탄약 |
| | `MaxAmmo_Blaster` | 6 | 최대 블래스터 탄약 |
| **Ammo-Bomb** | `Ammo_Bomb` | 0 | 현재 폭탄 탄약 |
| | `MaxAmmo_Bomb` | 150 | 최대 폭탄 탄약 |
| **Ammo-Tesla** | `Ammo_Tesla` | 0 | 현재 테슬라 탄약 |
| | `MaxAmmo_Tesla` | 150 | 최대 테슬라 탄약 |
| **Utility** | `DashCooldown` | 0.0f | 대시 쿨다운 |

**주요 기능:**
- GAS `AttributeSet`을 상속받아 구현
- 총 15개의 `FGameplayAttributeData` 속성을 4가지 카테고리로 분류 관리
- `ATTRIBUTE_ACCESSORS` 매크로를 사용하여 각 속성의 Getter/Setter 자동 생성
- `PostGameplayEffectExecute()` 함수로 속성값 변경 후 콜백 처리 (각 탄약별 클램핑)
- UI 업데이트를 위한 `FAttributeEvent` 델리게이트 선언

**속성 Getter/Setter 패턴:**

```cpp
// ATTRIBUTE_ACCESSORS 매크로가 자동 생성하는 함수들:

// 1. Property Getter (내부 데이터 구조 접근)
inline float GetHealth() const { return Health.GetBaseValue(); }

// 2. Value Getter (현재값)
float GetHealthAttribute() const;  // Attribute 인스턴스 반환

// 3. Value Setter (값 변경)
void SetHealth(float NewValue);

// 4. Value Initializer (초기화)
void InitHealth(float NewValue);

// 각 속성마다 동일한 패턴으로 자동 생성됨
// 예: SetAmmo_Blaster(), GetAmmo_Blaster(), InitAmmo_Blaster()
//     SetMaxAmmo_Tesla(), GetMaxAmmo_Tesla(), InitMaxAmmo_Tesla()
```

**초기화 코드:**

```cpp
URAC_AttributeSet::URAC_AttributeSet()
{
    // 1. 기본 능력치 (Base Stats)
    InitHealth(100.0f);
    InitMaxHealth(100.0f);
    InitExperience(0.0f);
    InitLevel(1);
    InitDamageMultiplier(1.f);
    
    // 2. 경제 시스템 (Economy)
    InitBolts(0);
    InitRaritanium(0);
    
    // 3. 탄약 창고 (Ammo Reserve)
    // 3.1 블래스터 탄약
    InitAmmo_Blaster(0);
    InitMaxAmmo_Blaster(6);
    
    // 3.2 폭탄 탄약
    InitAmmo_Bomb(0);
    InitMaxAmmo_Bomb(150);
    
    // 3.3 테슬라 탄약
    InitAmmo_Tesla(0);
    InitMaxAmmo_Tesla(150);
    
    // 4. 유틸리티 (Utility)
    InitDashCooldown(0.0f);
}
```

**현재 PostGameplayEffectExecute 구현:**

```cpp
void URAC_AttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);
    
    // 체력 변경 처리
    if (Data.EvaluatedData.Attribute == GetHealthAttribute())
    {
        OnHealthChanged.Broadcast(0.f, GetHealth());
    }
    
    // 볼트 변경 처리
    if (Data.EvaluatedData.Attribute == GetBoltsAttribute())
    {
        OnBoltsChanged.Broadcast(0.f, GetBolts());
    }
    // 추가로 탄약별 처리도 포함 가능 (OnAmmoChanged 델리게이트 활용)
}
```

**⚠️ 주목:** PostGameplayEffectExecute에서 각 탄약별로 값 클램핑 처리 권장
- `Ammo_Blaster` → 0 ~ MaxAmmo_Blaster
- `Ammo_Bomb` → 0 ~ MaxAmmo_Bomb
- `Ammo_Tesla` → 0 ~ MaxAmmo_Tesla

#### 2. **ARAC_PlayerState** (ASC 소유 및 관리)
**파일:** [Frameworks/RAC_PlayerState.h](Source/MyRatchet/Public/Frameworks/RAC_PlayerState.h)

**역할:**
- `IAbilitySystemInterface` 구현 (GAS 표준)
- `UAbilitySystemComponent` 생성 및 소유
- `URAC_AttributeSet` 생성 및 관리

**구현 코드:**
```cpp
ARAC_PlayerState::ARAC_PlayerState()
{
    // 1. ASC 생성
    AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    
    // 2. AttributeSet 생성
    AttributeSet = CreateDefaultSubobject<URAC_AttributeSet>(TEXT("AtributeSet"));
}

UAbilitySystemComponent* ARAC_PlayerState::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}
```

#### 3. **ARAC_CPP_Character** (캐릭터와 ASC 연동)
**파일:** [Characters/RAC_CPP_Character.h](Source/MyRatchet/Public/Characters/RAC_CPP_Character.h)

**추가된 기능:**
- `IAbilitySystemInterface` 구현
- `GetAbilitySystemComponent()` 함수 구현
- `PossessedBy()` 함수에서 ASC 초기화 (서버)
- `OnRep_PlayerState()` 함수에서 ASC 초기화 (클라이언트)

**바인딩 코드:**
```cpp
// 서버 바인딩
void ARAC_CPP_Character::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);
    
    ARAC_PlayerState* PS = GetPlayerState<ARAC_PlayerState>();
    if (PS)
    {
        PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);
    }
}

// 클라이언트 바인딩
void ARAC_CPP_Character::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();
    
    ARAC_PlayerState* PS = GetPlayerState<ARAC_PlayerState>();
    if (PS)
    {
        PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);
    }
}

// ASC 접근
class UAbilitySystemComponent* ARAC_CPP_Character::GetAbilitySystemComponent() const
{
    ARAC_PlayerState* PS = GetPlayerState<ARAC_PlayerState>();
    if (PS)
    {
        return PS->GetAbilitySystemComponent();
    }
    return nullptr;
}
```

---

## 시스템 아키텍처

### 계층 구조

```
APlayerController
    │
    ├─ Possesses (게임 시작 시)
    │
ARAC_CPP_Character (implements IAbilitySystemInterface)
    │
    ├─ GetPlayerState<ARAC_PlayerState>()
    │
ARAC_PlayerState (implements IAbilitySystemInterface)
    │
    ├─ UAbilitySystemComponent (ASC)
    │   │
    │   ├─ URAC_AttributeSet (모든 속성 보유)
    │   │   ├─ Health / MaxHealth
    │   │   ├─ Experience / Level
    │   │   ├─ Bolts / Raritanium
    │   │   ├─ Ammo_Blaster / MaxAmmo_Blaster
    │   │   ├─ Ammo_Bomb / MaxAmmo_Bomb
    │   │   ├─ Ammo_Tesla / MaxAmmo_Tesla
    │   │   ├─ DamageMultiplier
    │   │   └─ DashCooldown
    │   │
    │   ├─ GameplayAbility (향후 추가)
    │   │   ├─ Attack Ability
    │   │   ├─ Dash Ability
    │   │   └─ ...
    │   │
    │   └─ GameplayEffect (향후 추가)
    │       ├─ Damage Effect
    │       ├─ Heal Effect
    │       └─ ...
```

### 초기화 시퀀스

```
1. 게임 시작
   ↓
2. GameMode이 PlayerState 생성
   ├─ ARAC_PlayerState 생성
   ├─ ASC 생성 및 AttributeSet 생성
   │
3. Controller가 Character Possess
   ↓
4. PossessedBy() 호출 (서버)
   ├─ PlayerState 가져오기
   ├─ ASC->InitAbilityActorInfo(PlayerState, Character) 호출
   ├─ ASC가 PlayerState와 Character 정보로 초기화됨
   │
5. OnRep_PlayerState() 호출 (클라이언트)
   ├─ 위와 동일하게 초기화
   │
6. BeginPlay()
   ├─ GetAbilitySystemComponent() 검증
   ├─ ASC가 제대로 연결되었는지 확인
```

---

## 주요 클래스 설명

### 1. URAC_AttributeSet

**위치:** `Source/MyRatchet/Public/Characters/RAC_AttributeSet.h`

**기능:**
- GAS의 모든 게임 속성 정의
- 속성값 변경 시 콜백 처리
- UI 업데이트를 위한 이벤트 발생

---

## 사용 방법

### 1. 속성값 읽기

```cpp
// 캐릭터 코드에서
ARAC_PlayerState* PS = GetPlayerState<ARAC_PlayerState>();
if (PS && PS->GetAbilitySystemComponent())
{
    UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
    URAC_AttributeSet* Attributes = ASC->GetSet<URAC_AttributeSet>();
    
    // 기본 능력치 읽기
    float CurrentHealth = Attributes->GetHealth();
    float MaxHealth = Attributes->GetMaxHealth();
    int32 Level = Attributes->GetLevel();
    float Exp = Attributes->GetExperience();
    
    // 경제 시스템 읽기
    int32 Bolts = Attributes->GetBolts();
    int32 Raritanium = Attributes->GetRaritanium();
    
    // 탄약 창고 읽기 (각 탄약 유형별)
    float BlasterAmmo = Attributes->GetAmmo_Blaster();
    float BlasterMaxAmmo = Attributes->GetMaxAmmo_Blaster();
    
    float BombAmmo = Attributes->GetAmmo_Bomb();
    float BombMaxAmmo = Attributes->GetMaxAmmo_Bomb();
    
    float TeslaAmmo = Attributes->GetAmmo_Tesla();
    float TeslaMaxAmmo = Attributes->GetMaxAmmo_Tesla();
}
```

### 2. 속성값 변경 (GameplayEffect 이용)

**권장 방법 (GAS 표준):**

```cpp
// GameplayEffect 기반 변경 (향후 구현)
// 데이터 기반으로 체력 변경, 버프/디버프 관리 등이 가능

// 예시 (나중에 구현):
FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(DamageEffect, 1.0f, EffectContext);
ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
```

### 3. 속성 변경 이벤트 구독 (UI 업데이트)

```cpp
// UI 위젯에서
if (URAC_AttributeSet* Attributes = GetAttributeSet())
{
    // 속성 변경 시 콜백 바인딩
    Attributes->OnHealthChanged.AddDynamic(this, &UMyWidget::OnHealthChanged);
}

void UMyWidget::OnHealthChanged(float OldValue, float NewValue)
{
    // UI 업데이트
    HealthBar->SetPercent(NewValue / MaxHealth);
}
```

### 4. 게임 시작 시 초기 설정

**GameMode에서 PlayerState 클래스 설정:**

```cpp
// RAC_GameModeBase 또는 프로젝트 설정에서
ARAC_GameModeBase::ARAC_GameModeBase()
{
    // PlayerState 클래스 설정
    PlayerStateClass = ARAC_PlayerState::StaticClass();
    
    // DefaultPawnClass 등도 설정
}
```

또는 **프로젝트 설정:**
- Edit → Project Settings → Game → Classes
- Default Player State Class → ARAC_PlayerState 선택

---

## 수정 및 확장 방법

### 1. 새로운 속성 추가

**Step 1: RAC_AttributeSet.h에 속성 선언**

```cpp
// RAC_AttributeSet.h
// 적절한 카테고리에 추가
UPROPERTY(BlueprintReadOnly, Category="Attributes-NewCategory")
FGameplayAttributeData NewAttribute;
ATTRIBUTE_ACCESSORS(URAC_AttributeSet, NewAttribute);
```

**Step 2: RAC_AttributeSet.cpp에 초기화**

```cpp
// RAC_AttributeSet.cpp의 생성자
URAC_AttributeSet::URAC_AttributeSet()
{
    // 적절한 섹션에 추가
    InitNewAttribute(100.0f);  // 초기값
}
```

**Step 3: (선택) PostGameplayEffectExecute에 콜백 추가**

```cpp
void URAC_AttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);
    
    // 새 속성 처리
    if (Data.EvaluatedData.Attribute == GetNewAttributeAttribute())
    {
        // 필요시 값 클램핑
        SetNewAttribute(FMath::Clamp(GetNewAttribute(), 0.0f, MaxValue));
        
        // 이벤트 브로드캐스트 (펠리게이트 선언시)
        OnNewAttributeChanged.Broadcast(0.f, GetNewAttribute());
    }
}
```

**Step 4: (선택) 델리게이트 선언**

```cpp
// RAC_AttributeSet.h
UPROPERTY(BlueprintAssignable, Category="Events")
FAttributeEvent OnNewAttributeChanged;
```

**📝 카테고리 가이드:**
- `Attributes-State`: 캐릭터의 기본 상태 속성 (Health, Level 등)
- `Attributes-Economy`: 재화 관련 속성 (Bolts, Raritanium 등)
- `Attributes`: 일반 속성 (탄약 등)

### 2. 속성값 변경 규칙 추가

**PostGameplayEffectExecute 함수에서 클램핑:**

```cpp
void URAC_AttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);
    
    // 체력 변경 처리
    if (Data.EvaluatedData.Attribute == GetHealthAttribute())
    {
        // 체력을 0 ~ MaxHealth 범위로 제한
        SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
        OnHealthChanged.Broadcast(0.f, GetHealth());
    }
    
    // 재화 변경 처리
    if (Data.EvaluatedData.Attribute == GetBoltsAttribute())
    {
        // Bolts는 음수가 되지 않도록 제한
        SetBolts(FMath::Max(0, GetBolts()));
        OnBoltsChanged.Broadcast(0.f, GetBolts());
    }
    
    // 탄약 변경 처리 (블래스터)
    if (Data.EvaluatedData.Attribute == GetAmmo_BlasterAttribute())
    {
        SetAmmo_Blaster(FMath::Clamp(GetAmmo_Blaster(), 0.0f, GetMaxAmmo_Blaster()));
        OnAmmoChanged.Broadcast(GetAmmo_Blaster(), GetMaxAmmo_Blaster());
    }
    
    // 탄약 변경 처리 (폭탄)
    if (Data.EvaluatedData.Attribute == GetAmmo_BombAttribute())
    {
        SetAmmo_Bomb(FMath::Clamp(GetAmmo_Bomb(), 0.0f, GetMaxAmmo_Bomb()));
        OnAmmoChanged.Broadcast(GetAmmo_Bomb(), GetMaxAmmo_Bomb());
    }
    
    // 탄약 변경 처리 (테슬라)
    if (Data.EvaluatedData.Attribute == GetAmmo_TeslaAttribute())
    {
        SetAmmo_Tesla(FMath::Clamp(GetAmmo_Tesla(), 0.0f, GetMaxAmmo_Tesla()));
        OnAmmoChanged.Broadcast(GetAmmo_Tesla(), GetMaxAmmo_Tesla());
    }
}
```

### 3. GameplayAbility 추가 (향후)

**예시: 대시 능력**

```cpp
// RAC_DashAbility.h
UCLASS()
class MYRATCHET_API URAC_DashAbility : public UGameplayAbility
{
    GENERATED_BODY()

public:
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, ...);
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, ...);
};

// RAC_DashAbility.cpp
void URAC_DashAbility::ActivateAbility(...)
{
    // 대시 로직 구현
}
```

**PlayerState에서 능력 등록:**

```cpp
// RAC_PlayerState.cpp 생성자
ARAC_PlayerState::ARAC_PlayerState()
{
    // ...
    
    // 능력 등록 (나중)
    // AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(URAC_DashAbility::StaticClass()));
}
```

### 4. GameplayEffect 추가 (향후)

**예시: 데미지 효과**

```cpp
// 블루프린트에서 생성 또는 코드로 생성
FGameplayEffectSpecHandle DamageEffect = ASC->MakeOutgoingSpec(
    DamageEffectClass,  // 효과 클래스
    AbilityLevel,        // 레벨
    EffectContext        // 컨텍스트
);

// 효과 적용
ASC->ApplyGameplayEffectSpecToSelf(*DamageEffect.Data.Get());
```

---

## 주의사항

### ⚠️ 중요한 주의사항

#### 1. **PlayerState 생성 타이밍**

```cpp
// ❌ 잘못된 예: PlayerState가 아직 없을 때 접근
void AMyCharacter::BeginPlay()
{
    ARAC_PlayerState* PS = GetPlayerState<ARAC_PlayerState>();  // null일 수 있음!
}

// ✅ 올바른 예: PossessedBy/OnRep_PlayerState에서 처리
void AMyCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);
    ARAC_PlayerState* PS = GetPlayerState<ARAC_PlayerState>();  // 이 시점에는 존재
}
```

#### 2. **GetAbilitySystemComponent() 검증**

```cpp
// ❌ 잘못된 예
void AMyCharacter::TakeDamage(float Damage)
{
    GetAbilitySystemComponent()->GetNumericAttribute(Health);  // null check 없음!
}

// ✅ 올바른 예
void AMyCharacter::TakeDamage(float Damage)
{
    if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
    {
        ASC->ModifyAttributeValue(Health, Damage);
    }
}
```

#### 3. **네트워크 리플리케이션**

현재 ASC는 싱글플레이를 기준으로 설정되었습니다.

**멀티플레이를 위한 수정:**

```cpp
// RAC_PlayerState.cpp
ARAC_PlayerState::ARAC_PlayerState()
{
    AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    
    // 멀티플레이인 경우 다음 추가
    AbilitySystemComponent->SetIsReplicated(true);  // 네트워크 동기화
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);  // 효율적인 동기화
}
```

#### 4. **AttributeSet 초기값 설정**

속성의 초기값은 **RAC_AttributeSet 생성자에서만 설정**하세요.

```cpp
// ❌ 잘못된 예: 이렇게 하면 안 됨
FGameplayAttributeData Health = 150.0f;

// ✅ 올바른 예: 생성자에서만
URAC_AttributeSet::URAC_AttributeSet()
{
    InitHealth(150.0f);
}
```

#### 5. **PostGameplayEffectExecute에서의 처리**

```cpp
void URAC_AttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    // 이 함수는 GameplayEffect 적용 후 호출됨
    // 여기서만 속성값을 최종 조정하세요
    
    if (Data.EvaluatedData.Attribute == GetHealthAttribute())
    {
        // 최대값/최소값 제한 (필수)
        SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
    }
}
```

---

## 체크리스트

### 프로젝트 시작 전 확인사항

- [ ] GameMode 설정에서 PlayerStateClass = ARAC_PlayerState 확인
- [ ] RAC_CPP_Character가 IAbilitySystemInterface 상속 확인
- [ ] PossessedBy()와 OnRep_PlayerState()에서 InitAbilityActorInfo() 호출 확인
- [ ] BeginPlay()에서 GetAbilitySystemComponent() 검증 확인

### 기능 추가 시 체크리스트

#### 새로운 속성 추가 시
- [ ] RAC_AttributeSet.h에 FGameplayAttributeData 선언 (적절한 카테고리 선택)
- [ ] ATTRIBUTE_ACCESSORS 매크로 적용
- [ ] RAC_AttributeSet.cpp 생성자에 초기값 설정 (적절한 섹션에 추가)
- [ ] PostGameplayEffectExecute에 값 클램핑 로직 추가
- [ ] (선택) 속성 변경 이벤트용 델리게이트 선언
- [ ] (선택) OnAmmoChanged 또는 새로운 이벤트 브로드캐스트

#### 새로운 GameplayAbility 추가 시
- [ ] UGameplayAbility 상속한 클래스 생성
- [ ] ActivateAbility() 구현
- [ ] EndAbility() 구현
- [ ] ARAC_PlayerState에서 GiveAbility() 호출

#### UI 연동 시
- [ ] AttributeSet의 FAttributeEvent에 바인딩
- [ ] 속성값 변경 시 UI 업데이트 콜백 구현

---

## 파일 변경 요약

| 파일 | 변경 유형 | 내용 |
|------|---------|------|
| `RAC_AttributeSet.h` | 신규/수정 | AttributeSet 헤더 (15개 속성, 4개 카테고리) |
| `RAC_AttributeSet.cpp` | 신규/수정 | AttributeSet 구현 (초기화 및 값 클램핑, 각 탄약별 처리) |
| `RAC_PlayerState.h` | 신규 | PlayerState 헤더 |
| `RAC_PlayerState.cpp` | 신규 | PlayerState 구현 (ASC/AttributeSet 생성) |
| `RAC_CPP_Character.h` | 수정 | IAbilitySystemInterface 상속, GetAbilitySystemComponent() 추가 |
| `RAC_CPP_Character.cpp` | 수정 | BeginPlay(), PossessedBy(), OnRep_PlayerState(), GetAbilitySystemComponent() 구현 |
| `RAC_Cpp_GameMode.h/.cpp` | 기존 유지 | PlayerState 클래스 자동 할당 설정 필요 (게임모드 설정) |

---

## 참고 자료

### GAS 공식 문서
- [Unreal Engine GAS Documentation](https://docs.unrealengine.com/5.0/en-US/gameplay-ability-system-in-unreal-engine/)

### 주요 클래스
- `UAttributeSet` - 속성 정의 기본 클래스
- `UAbilitySystemComponent` - GAS 핵심 컴포넌트
- `UGameplayAbility` - 능력 기본 클래스
- `UGameplayEffect` - 속성 변경 효과
- `IAbilitySystemInterface` - GAS 표준 인터페이스

### 현재 프로젝트 파일
- [RAC_AttributeSet.h/cpp](Source/MyRatchet/Public/Characters/RAC_AttributeSet.h)
- [RAC_PlayerState.h/cpp](Source/MyRatchet/Public/Frameworks/RAC_PlayerState.h)
- [RAC_CPP_Character.h/cpp](Source/MyRatchet/Public/Characters/RAC_CPP_Character.h)

---

**작성일:** 2026년 1월 30일  
**업데이트:** 2026년 1월 30일 (AttributeSet 15개 속성 & 4개 카테고리)  
**프로젝트:** MyRatchet  
**엔진:** Unreal Engine 5
