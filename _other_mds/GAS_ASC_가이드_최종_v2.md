# MyRatchet 프로젝트 - GAS(Gameplay Ability System) & ASC(Ability System Component) 완벽 가이드 (v2)

## 📋 목차
1. [개요](#개요)
2. [GAS와 ASC의 개념](#gas와-asc의-개념)
3. [현재 구현 상황](#현재-구현-상황)
4. [시스템 아키텍처](#시스템-아키텍처)
5. [주요 클래스 설명](#주요-클래스-설명)
6. [사용 방법](#사용-방법)
7. [무기 공용 탄약 + 캐싱 방식](#무기-공용-탄약--캐싱-방식)
8. [무기별 적용 Ability 목록 관리](#무기별-적용-ability-목록-관리)
9. [수정 및 확장 방법](#수정-및-확장-방법)
10. [주의사항](#주의사항)

---

## 개요

이 문서는 MyRatchet 프로젝트에 구현된 **GAS(Gameplay Ability System)**와 **ASC(Ability System Component)** 시스템에 대한 가이드입니다.

**목표:**
- 플레이어 캐릭터의 모든 게임 속성(체력, 탄약, 경험치 등)을 중앙화하여 관리
- GameplayEffect와 GameplayAbility를 통한 데이터 기반 능력 시스템 구축
- 싱글플레이 기준 설계

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

**현재 관리하는 속성(예시):**

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

// 3. 공용 탄약 (Shared Ammo)
FGameplayAttributeData Ammo;             // 현재 탄약 (공용)
FGameplayAttributeData MaxAmmo;          // 최대 탄약 (공용)

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
| **Shared Ammo** | `Ammo` | 0 | 공용 현재 탄약 |
| | `MaxAmmo` | 0 | 공용 최대 탄약 |
| **Utility** | `DashCooldown` | 0.0f | 대시 쿨다운 |

**주요 기능:**
- GAS `AttributeSet`을 상속받아 구현
- `ATTRIBUTE_ACCESSORS` 매크로를 사용하여 각 속성의 Getter/Setter 자동 생성
- `PostGameplayEffectExecute()` 함수로 속성값 변경 후 콜백 처리
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
```

**초기화 코드 (예시):**

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
    
    // 3. 공용 탄약 (Shared Ammo)
    InitAmmo(0);
    InitMaxAmmo(0);
    
    // 4. 유틸리티 (Utility)
    InitDashCooldown(0.0f);
}
```

**PostGameplayEffectExecute (공용 탄약 처리 예시):**

```cpp
void URAC_AttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);

    if (Data.EvaluatedData.Attribute == GetHealthAttribute())
    {
        SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
        OnHealthChanged.Broadcast(0.f, GetHealth());
    }

    if (Data.EvaluatedData.Attribute == GetBoltsAttribute())
    {
        SetBolts(FMath::Max(0, GetBolts()));
        OnBoltsChanged.Broadcast(0.f, GetBolts());
    }

    if (Data.EvaluatedData.Attribute == GetAmmoAttribute())
    {
        SetAmmo(FMath::Clamp(GetAmmo(), 0.0f, GetMaxAmmo()));
        OnAmmoChanged.Broadcast(GetAmmo(), GetMaxAmmo());
    }

    if (Data.EvaluatedData.Attribute == GetMaxAmmoAttribute())
    {
        SetMaxAmmo(FMath::Max(0.0f, GetMaxAmmo()));
        SetAmmo(FMath::Clamp(GetAmmo(), 0.0f, GetMaxAmmo()));
        OnAmmoChanged.Broadcast(GetAmmo(), GetMaxAmmo());
    }
}
```

#### 2. **ARAC_PlayerState** (ASC 소유 및 관리)
**파일:** [Frameworks/RAC_PlayerState.h](Source/MyRatchet/Public/Frameworks/RAC_PlayerState.h)

**역할:**
- `IAbilitySystemInterface` 구현 (GAS 표준)
- `UAbilitySystemComponent` 생성 및 소유
- `URAC_AttributeSet` 생성 및 관리

#### 3. **ARAC_CPP_Character** (캐릭터와 ASC 연동)
**파일:** [Characters/RAC_CPP_Character.h](Source/MyRatchet/Public/Characters/RAC_CPP_Character.h)

**추가된 기능:**
- `IAbilitySystemInterface` 구현
- `GetAbilitySystemComponent()` 함수 구현
- `PossessedBy()` 함수에서 ASC 초기화
- `OnRep_PlayerState()` 함수에서 ASC 초기화

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
    │   │   ├─ Ammo / MaxAmmo  (공용)
    │   │   ├─ DamageMultiplier
    │   │   └─ DashCooldown
```

---

## 사용 방법

### 1. 속성값 읽기

```cpp
ARAC_PlayerState* PS = GetPlayerState<ARAC_PlayerState>();
if (PS && PS->GetAbilitySystemComponent())
{
    UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
    URAC_AttributeSet* Attributes = ASC->GetSet<URAC_AttributeSet>();
    
    float CurrentHealth = Attributes->GetHealth();
    float MaxHealth = Attributes->GetMaxHealth();
    int32 Level = Attributes->GetLevel();
    float Exp = Attributes->GetExperience();

    float CurrentAmmo = Attributes->GetAmmo();
    float MaxAmmo = Attributes->GetMaxAmmo();
}
```

### 2. 속성값 변경 (GameplayEffect 이용)

```cpp
FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(DamageEffect, 1.0f, EffectContext);
ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
```

### 3. 속성 변경 이벤트 구독 (UI 업데이트)

```cpp
if (URAC_AttributeSet* Attributes = GetAttributeSet())
{
    Attributes->OnHealthChanged.AddDynamic(this, &UMyWidget::OnHealthChanged);
    Attributes->OnAmmoChanged.AddDynamic(this, &UMyWidget::OnAmmoChanged);
}
```

---

## 무기 공용 탄약 + 캐싱 방식

### 목적
- 무기별 `Ammo/MaxAmmo` 어트리뷰트를 분리하지 않음
- **공용 `Ammo/MaxAmmo`**만 유지
- 무기 교체 시 **기존 무기의 잔탄을 캐싱**하고, **새 무기의 캐시 값으로 덮어쓰기**

### 데이터 구조 (예시)

```cpp
USTRUCT(BlueprintType)
struct FWeaponAmmoCache
{
    GENERATED_BODY()

    UPROPERTY()
    float Ammo = 0.0f;
};

UPROPERTY()
TMap<FName, FWeaponAmmoCache> WeaponAmmoCacheMap;
```

### 동작 흐름
1) 무기 교체 직전: 현재 `Ammo`만 `WeaponAmmoCacheMap[OldWeaponId]`에 저장
2) 무기 교체 직후: `WeaponAmmoCacheMap[NewWeaponId]`가 있으면 그 값으로 `Ammo` 설정
3) 캐시가 없으면 해당 무기의 기본 `Ammo`로 초기화
4) `MaxAmmo`는 **GameplayEffect로 적용** (무기 스왑 시 해당 무기 GE를 적용)
5) 싱글플레이라서 **서버/복제 고려 없음**

### 의사 코드 (캐릭터 혹은 플레이어 스테이트)

```cpp
void URAC_WeaponManager::SwapWeapon(FName NewWeaponId)
{
    // 1) 현재 무기 캐시
    if (CurrentWeaponId != NAME_None)
    {
        FWeaponAmmoCache& Cache = WeaponAmmoCacheMap.FindOrAdd(CurrentWeaponId);
        Cache.Ammo = Attributes->GetAmmo();
    }

    // 2) 새 무기 적용
    const FWeaponAmmoCache* NewCache = WeaponAmmoCacheMap.Find(NewWeaponId);
    if (NewCache)
    {
        Attributes->SetAmmo(NewCache->Ammo);
    }
    else
    {
        const float DefaultAmmo = GetWeaponDefaultAmmo(NewWeaponId);
        Attributes->SetAmmo(DefaultAmmo);
    }

    // 3) MaxAmmo는 무기별 GE로 적용
    ApplyWeaponMaxAmmoEffect(NewWeaponId);

    CurrentWeaponId = NewWeaponId;
}
```

### 장점
- AttributeSet이 단순해짐
- 무기 추가/삭제 시 어트리뷰트 추가 불필요
- 캐싱으로 무기별 잔탄 유지 가능

---

## 무기별 적용 Ability 목록 관리

### 목적
- 무기마다 적용되는 GA 목록을 데이터로 관리
- 무기 교체 시 **기존 무기 Ability 제거**, **새 무기 Ability 부여**
- 싱글플레이 기준으로 간단하게 처리

### 데이터 구조 (예시)

```cpp
USTRUCT(BlueprintType)
struct FWeaponAbilitySet
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly)
    TArray<TSubclassOf<UGameplayAbility>> Abilities;
};

UPROPERTY(EditDefaultsOnly)
TMap<FName, FWeaponAbilitySet> WeaponAbilityMap;

// 현재 무기에 부여된 Ability 핸들 캐시
UPROPERTY()
TArray<FGameplayAbilitySpecHandle> GrantedWeaponAbilityHandles;
```

### 동작 흐름
1) 무기 교체 직전: 현재 무기에서 **부여된 Ability 핸들 제거**
2) 무기 교체 직후: 새 무기의 Ability 목록을 **ASC에 부여**하고 핸들 캐싱
3) 무기별 Ability는 **무기 데이터(데이터 에셋/테이블)와 연동** 가능

### 의사 코드 (ASC 보유 클래스에서 처리)

```cpp
void URAC_WeaponManager::ApplyWeaponAbilities(FName WeaponId)
{
    // 1) 기존 무기 Ability 제거
    if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
    {
        for (const FGameplayAbilitySpecHandle& Handle : GrantedWeaponAbilityHandles)
        {
            ASC->ClearAbility(Handle);
        }
        GrantedWeaponAbilityHandles.Reset();

        // 2) 새 무기 Ability 부여
        const FWeaponAbilitySet* AbilitySet = WeaponAbilityMap.Find(WeaponId);
        if (AbilitySet)
        {
            for (TSubclassOf<UGameplayAbility> AbilityClass : AbilitySet->Abilities)
            {
                if (AbilityClass)
                {
                    FGameplayAbilitySpec Spec(AbilityClass, 1);
                    FGameplayAbilitySpecHandle Handle = ASC->GiveAbility(Spec);
                    GrantedWeaponAbilityHandles.Add(Handle);
                }
            }
        }
    }
}
```

### 주의
- 무기 교체 타이밍에서 **탄약 적용 → Ability 적용** 순서를 유지하면 UI/입력 동기화가 편함
- 단일 플레이어라서 복제 고려 없음

---

## 수정 및 확장 방법

### 1. 새로운 속성 추가

```cpp
UPROPERTY(BlueprintReadOnly, Category="Attributes-NewCategory")
FGameplayAttributeData NewAttribute;
ATTRIBUTE_ACCESSORS(URAC_AttributeSet, NewAttribute);
```

### 2. 속성값 변경 규칙 추가

```cpp
if (Data.EvaluatedData.Attribute == GetNewAttributeAttribute())
{
    SetNewAttribute(FMath::Clamp(GetNewAttribute(), 0.0f, MaxValue));
    OnNewAttributeChanged.Broadcast(0.f, GetNewAttribute());
}
```

---

## 주의사항

### ⚠️ 중요한 주의사항

#### 1. **PlayerState 생성 타이밍**

```cpp
void AMyCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);
    ARAC_PlayerState* PS = GetPlayerState<ARAC_PlayerState>();
}
```

#### 2. **GetAbilitySystemComponent() 검증**

```cpp
if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
{
    ASC->ModifyAttributeValue(Health, Damage);
}
```

#### 3. **AttributeSet 초기값 설정**

```cpp
URAC_AttributeSet::URAC_AttributeSet()
{
    InitHealth(150.0f);
}
```

---

## 파일 변경 요약

| 파일 | 변경 유형 | 내용 |
|------|---------|------|
| `RAC_AttributeSet.h` | 수정 | 공용 탄약(`Ammo/MaxAmmo`)만 유지, 무기별 탄약 제거 |
| `RAC_AttributeSet.cpp` | 수정 | 공용 탄약 초기화/클램핑 로직 반영 |
| (선택) 무기 매니저/캐릭터 | 수정 | 무기별 탄약 캐싱/스왑 로직 추가 |

---

**작성일:** 2026년 1월 30일  
**업데이트:** 2026년 1월 30일 (공용 탄약 + 무기별 캐싱 방식 반영)  
**프로젝트:** MyRatchet  
**엔진:** Unreal Engine 5
