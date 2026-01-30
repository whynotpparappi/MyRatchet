# Add HUD

## 개요
플레이어의 체력, 재화(Bolts), 탄약 등의 게임 데이터를 실시간으로 표시하는 HUD(Head-Up Display) 시스템을 구축하는 작업입니다. GAS와 델리게이트를 활용하여 데이터 변경을 감지하고 UI를 동적으로 업데이트합니다.

## 주요 변경 사항

### 1. 새로운 클래스 생성

#### `URAC_HUD_Widget` (신규)
- **경로**: `Source/MyRatchet/Public/Widgets/RAC_HUD_Widget.h` / `.cpp`
- **역할**: 게임의 모든 HUD UI 요소 관리 및 업데이트

**주요 기능:**
- PlayerState의 AttributeSet에 바인드되어 속성 변경 감지
- 텍스트 및 프로그래스 바로 게임 상태 표시
- 델리게이트를 통한 실시간 UI 업데이트

**주요 메서드:**
```cpp
virtual void NativeConstruct() override;        // 위젯 초기화
virtual void NativeDestruct() override;         // 위젯 제거 시 정리

void BindToAttributeSet();                      // AttributeSet에 델리게이트 바인드
void OnHealthChanged(float OldValue, float NewValue);   // 체력 변경 콜백
void OnBoltsChanged(float OldValue, float NewValue);    // 볼츠 변경 콜백
void OnAmmoChanged(float OldValue, float NewValue);     // 탄약 변경 콜백
void UpdateHealthUI(float CurrentHealth, float MaxHealth);   // 체력 UI 업데이트
void UpdateBoltsUI(float CurrentBolts);                      // 볼츠 UI 업데이트
void UpdateAmmoUI(float CurrentAmmo, float MaxAmmo);         // 탄약 UI 업데이트
```

**UI 요소들 (BindWidget):**

| UI 요소 | 타입 | 설명 |
|---------|------|------|
| `HealthText` | UTextBlock | 현재 체력 수치 표시 (정수) |
| `HealthBar` | UProgressBar | 체력 바 (0.0 ~ 1.0) |
| `BoltsText` | UTextBlock | 현재 볼츠(재화) 수치 표시 |
| `AmmoText` | UTextBlock | 현재/최대 탄약 표시 (예: "15 / 20") |
| `AmmoBar` | UProgressBar | 탄약 바 (0.0 ~ 1.0) |

**주요 멤버 변수:**
```cpp
ARAC_PlayerState* OwningPlayerState;    // 소유 플레이어의 PlayerState
```

### 2. RAC_PlayerController 클래스 수정

#### 추가된 항목
- **HUD Widget 관리**:
  - `TSubclassOf<URAC_HUD_Widget> HUDWidgetClass`: 블루프린트에서 설정할 HUD 위젯 클래스
  - `URAC_HUD_Widget* HUDWidget`: 생성된 HUD 위젯 인스턴스

**BeginPlay에서 HUD 생성:**
```cpp
if (HUDWidgetClass && IsLocalController())
{
    HUDWidget = CreateWidget<URAC_HUD_Widget>(this, HUDWidgetClass);
    if (HUDWidget)
    {
        HUDWidget->AddToViewport();
    }
}
```
- 로컬 플레이어 컨트롤러일 때만 HUD 위젯 생성
- 생성된 위젯을 뷰포트에 추가하여 화면에 표시

### 3. RAC_AttributeSet 클래스 수정

#### 추가된 항목
- **델리게이트 선언**:
  ```cpp
  FAttributeChangeDelegate OnHealthChanged;   // 체력 변경 델리게이트
  FAttributeChangeDelegate OnBoltsChanged;    // 볼츠 변경 델리게이트
  ```
  - `FAttributeChangeDelegate`는 `float OldValue, float NewValue` 시그니처 가진 동적 델리게이트

**PostGameplayEffectExecute에서 호출:**
- Health 속성 변경 시 `OnHealthChanged` 델리게이트 브로드캐스트
- Bolts 속성 변경 시 `OnBoltsChanged` 델리게이트 브로드캐스트
- Health는 [0, MaxHealth] 범위로 클램핑

### 4. RAC_CPP_Character 클래스 수정

#### 추가된 항목
- **기본 체력 초기화**:
  ```cpp
  if (ARAC_PlayerState* PS = GetPlayerState<ARAC_PlayerState>())
  {
      if (PS->AttributeSet)
      {
          PS->AttributeSet->SetHealth(PS->AttributeSet->GetMaxHealth());
      }
  }
  ```
- 게임 시작 시 캐릭터의 체력을 최대 체력으로 설정

## 데이터 흐름도

```
AttributeSet (Health, Bolts, Ammo 변경)
    ↓
FAttributeChangeDelegate 브로드캐스트
    ↓
URAC_HUD_Widget::OnHealthChanged / OnBoltsChanged / OnAmmoChanged 콜백
    ↓
UpdateHealthUI / UpdateBoltsUI / UpdateAmmoUI 함수 호출
    ↓
UI 요소 (TextBlock, ProgressBar) 업데이트
    ↓
화면에 실시간 표시
```

## 구현 세부사항

### NativeConstruct 초기화 과정
1. `GetOwningPlayer()` 로 플레이어 컨트롤러 획득
2. PlayerController → ARAC_PlayerState로 캐스트
3. PlayerState가 유효하면 `BindToAttributeSet()` 호출

### BindToAttributeSet 바인딩
1. PlayerState의 AttributeSet 유효성 확인
2. `OnHealthChanged`, `OnBoltsChanged` 델리게이트에 콜백 함수 바인드
3. 초기 UI 업데이트 (현재 값으로 세팅)

### UpdateHealthUI 동작
```cpp
void URAC_HUD_Widget::UpdateHealthUI(float CurrentHealth, float MaxHealth)
{
    if (HealthText)
    {
        // 체력을 정수로 변환하여 텍스트 표시
        HealthText->SetText(FText::AsNumber(FMath::RoundToInt(CurrentHealth)));
    }

    if (HealthBar && MaxHealth > 0.f)
    {
        // 0.0 ~ 1.0 범위의 Percent 설정
        HealthBar->SetPercent(CurrentHealth / MaxHealth);
    }
}
```

### UpdateAmmoUI 동작
```cpp
void URAC_HUD_Widget::UpdateAmmoUI(float CurrentAmmo, float MaxAmmo)
{
    if (AmmoText)
    {
        // "현재 / 최대" 형식으로 표시 (예: "15 / 20")
        FString AmmoString = FString::Printf(TEXT("%d / %d"), 
            FMath::RoundToInt(CurrentAmmo), 
            FMath::RoundToInt(MaxAmmo));
        AmmoText->SetText(FText::FromString(AmmoString));
    }

    if (AmmoBar && MaxAmmo > 0.f)
    {
        AmmoBar->SetPercent(CurrentAmmo / MaxAmmo);
    }
}
```

## 블루프린트 설정

HUD 위젯이 제대로 작동하려면 다음과 같이 설정해야 합니다:

1. **HUDWidgetClass 설정**
   - `BP_RAC_PlayerController`의 `HUD Widget Class` 에 `WBP_RAC_HUD_Widget` 할당

2. **UMG 위젯 바인딩**
   - `WBP_RAC_HUD_Widget` 의 각 UI 요소를 C++ 클래스의 다음과 일치하도록 이름 설정:
     - `HealthText`
     - `HealthBar`
     - `BoltsText`
     - `AmmoText`
     - `AmmoBar`

## 주요 특징

- **실시간 업데이트**: GAS 델리게이트를 활용하여 속성 변경 시 즉시 UI 반영
- **자동 정규화**: ProgressBar는 자동으로 0.0 ~ 1.0 범위로 정규화
- **안전한 포인터 체크**: 모든 UI 요소에 대해 null 체크 수행
- **분리된 책임**: HUD는 PlayerController에서 관리되어 캐릭터와 독립적
- **메모리 관리**: NativeDestruct에서 델리게이트 정리로 메모리 누수 방지

## 파일 변경 사항

| 파일 | 변경 사항 |
|------|---------|
| `RAC_HUD_Widget.h` | 신규 생성 |
| `RAC_HUD_Widget.cpp` | 신규 생성 (114줄) |
| `RAC_PlayerController.h` | HUD Widget 멤버 추가 |
| `RAC_PlayerController.cpp` | BeginPlay에서 HUD 생성 |
| `RAC_AttributeSet.h` | 델리게이트 선언 |
| `RAC_AttributeSet.cpp` | PostGameplayEffectExecute 수정 (델리게이트 브로드캐스트) |
| `RAC_CPP_Character.cpp` | 초기 체력 설정 로직 추가 |
| `WBP_RAC_HUD_Widget.uasset` | 신규 UMG 위젯 블루프린트 |
