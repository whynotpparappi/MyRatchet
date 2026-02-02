# add WeaponData + WeaponManger + adapt GA,GE

## 요약
무기 데이터와 무기 매니저를 추가하고, 무기 교체 시 GA/GE 적용 흐름에 맞게 구조를 정리했다. 탄약 초기값과 무기 데이터 구조도 함께 조정했다.

## 변경 사항
- `URAC_WeaponManager` 컴포넌트 신규 추가 및 무기 스왑 흐름 구현
  - 기존 무기 능력 제거, 스탯 GE 제거, 탄약 캐시 저장/복원
  - 새 무기 스탯 GE 적용 후 탄약 복구 → 능력 부여 순서 준수
- `URAC_WeaponData` 구조 개편
  - `Abilities` → `AbilitiesToGrant`로 명확화
  - 전투 사양(`bIsFullAuto`, `bIsParabolic`, `DamageValue`, `DefaultAmmo`) 추가
  - 불필요한 `FWeaponCombatSpecs` 구조체 제거
- 기본 탄약 초기값을 100으로 조정
- 테스트용 GA/GE 및 데이터 에셋 추가(콘텐츠 리소스)

## 파일
- Source/MyRatchet/Private/Weapons/RAC_WeaponManager.cpp
- Source/MyRatchet/Public/Weapons/RAC_WeaponManager.h
- Source/MyRatchet/Public/Weapons/RAC_WeaponData.h
- Source/MyRatchet/Private/Characters/RAC_AttributeSet.cpp
- Content/_M_RAC/Test/GA_Fire.uasset
- Content/_M_RAC/Test/GE_BlastStats.uasset
- Content/_M_RAC/Test/GE_BombStats.uasset
- Content/_M_RAC/Test/Blast.uasset
- Content/_M_RAC/Test/Bomb.uasset
