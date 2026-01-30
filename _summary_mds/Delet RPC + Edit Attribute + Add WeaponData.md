# Delet RPC + Edit Attribute + Add WeaponData

## 개요
싱글플레이 기준으로 네트워크 RPC를 제거하고, Attribute 구조를 정리/확장했으며, 무기 데이터 에셋(WeaponData)을 추가한 변경사항을 정리합니다.

## 주요 변경 사항

### 1) 사격 RPC 제거 및 로컬 처리 전환
- 기존 서버 RPC 호출 흐름을 제거하고, 로컬에서 바로 사격을 처리하도록 변경했습니다.
- 싱글플레이 기준으로 네트워크 오버헤드를 줄이는 목적입니다.

**변경 요약**
- `ServerHandleShoot` RPC 제거
- `HandleShoot` 로컬 함수로 통합
- 탄약 소모 로직은 동일하되 `Ammo` 속성으로 변경

관련 파일:
- [Source/MyRatchet/Public/Characters/RAC_CPP_Character.h](Source/MyRatchet/Public/Characters/RAC_CPP_Character.h)
- [Source/MyRatchet/Private/Characters/RAC_CPP_Character.cpp](Source/MyRatchet/Private/Characters/RAC_CPP_Character.cpp)

### 2) Attribute 구조 정리 및 신규 속성 추가
- 속성 범주를 정리하고, 초기값/클램프 로직을 보강했습니다.
- 탄약 속성 명칭을 `CurrentAmmo` → `Ammo`로 정리했습니다.

**추가/변경된 속성**
- Base: `Health`, `MaxHealth`, `Experience`, `Level`, `DamageMultiplier`
- Economy: `Bolts`, `Raritanium`
- Ammo: `Ammo`, `MaxAmmo`

**초기화 변경**
- 체력: 100
- 레벨: 1
- 데미지 배율: 1
- Bolts/Raritanium/Ammo 기본값 0
- MaxAmmo 150

**PostGameplayEffectExecute 클램프 보강**
- Health: 0 ~ MaxHealth
- Bolts/MaxAmmo: 음수 방지
- Ammo: 0 ~ MaxAmmo

관련 파일:
- [Source/MyRatchet/Public/Characters/RAC_AttributeSet.h](Source/MyRatchet/Public/Characters/RAC_AttributeSet.h)
- [Source/MyRatchet/Private/Characters/RAC_AttributeSet.cpp](Source/MyRatchet/Private/Characters/RAC_AttributeSet.cpp)

### 3) 싱글플레이 기준 ASC 리플리케이션 제거
- PlayerState의 AbilitySystemComponent에서 리플리케이션을 비활성화했습니다.

관련 파일:
- [Source/MyRatchet/Private/Frameworks/RAC_PlayerState.cpp](Source/MyRatchet/Private/Frameworks/RAC_PlayerState.cpp)

### 4) WeaponData 데이터 에셋 추가
- 무기별 데이터 정의용 `UPrimaryDataAsset`을 추가했습니다.
- 무기 태그, 부여할 어빌리티 목록, 스탯 수정 GE, 비주얼(메시/머티리얼) 참조를 구성합니다.

**핵심 구성**
- `FWeaponCombatSpecs` 구조체
- `URAC_WeaponData` 에셋 클래스

관련 파일:
- [Source/MyRatchet/Public/Weapons/RAC_WeaponData.h](Source/MyRatchet/Public/Weapons/RAC_WeaponData.h)
- [Source/MyRatchet/Private/Weapons/RAC_WeaponData.cpp](Source/MyRatchet/Private/Weapons/RAC_WeaponData.cpp)

## 에셋 변경 요약
- 신규 테스트용 GameplayEffect 에셋 추가
  - GE_Ammo, GE_Bullet
- 기존 무기 블루프린트/태그 관련 에셋 일부 제거

## 변경 파일 요약

| 구분 | 파일 | 내용 |
|---|---|---|
| 수정 | [Source/MyRatchet/Public/Characters/RAC_CPP_Character.h](Source/MyRatchet/Public/Characters/RAC_CPP_Character.h) | 사격 RPC 제거, `HandleShoot` 추가 |
| 수정 | [Source/MyRatchet/Private/Characters/RAC_CPP_Character.cpp](Source/MyRatchet/Private/Characters/RAC_CPP_Character.cpp) | 로컬 사격 처리 전환, `Ammo` 사용 |
| 수정 | [Source/MyRatchet/Public/Characters/RAC_AttributeSet.h](Source/MyRatchet/Public/Characters/RAC_AttributeSet.h) | 속성 카테고리 정리 및 신규 속성 추가 |
| 수정 | [Source/MyRatchet/Private/Characters/RAC_AttributeSet.cpp](Source/MyRatchet/Private/Characters/RAC_AttributeSet.cpp) | 초기값 변경, 클램프/브로드캐스트 보강 |
| 수정 | [Source/MyRatchet/Private/Frameworks/RAC_PlayerState.cpp](Source/MyRatchet/Private/Frameworks/RAC_PlayerState.cpp) | ASC 리플리케이션 제거 |
| 추가 | [Source/MyRatchet/Public/Weapons/RAC_WeaponData.h](Source/MyRatchet/Public/Weapons/RAC_WeaponData.h) | WeaponData 데이터 에셋 정의 |
| 추가 | [Source/MyRatchet/Private/Weapons/RAC_WeaponData.cpp](Source/MyRatchet/Private/Weapons/RAC_WeaponData.cpp) | WeaponData 구현 파일 |
