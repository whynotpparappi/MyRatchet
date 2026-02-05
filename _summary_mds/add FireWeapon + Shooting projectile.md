# add FireWeapon + Shooting projectile

## 개요
FireWeapon(자동 사격)와 프로젝타일 발사 로직을 실제 동작하도록 구현하고, 무기 매니저 초기화/무기 전환(Tab) 입력, HUD 탄약 갱신 바인딩을 보강한 커밋.

## 주요 변경 사항 (상세)
- **GA_AutoFire**
  - `FireOnce()`에서 실제 발사 처리 구현.
    - 화면 중앙 기준 크로스헤어 레이를 구성하고 `ECC_Visibility` 라인트레이스로 목표 지점 결정.
    - 총구 소켓(`MuzzleSocketName`) 기준으로 스폰 위치/회전을 계산하고 프로젝타일 스폰.
    - 프로젝타일에 `UProjectileMovementComponent`가 있으면 `ProjectileSpeed` 기반 속도 설정.
  - 발사 유틸 추가
    - `GetCrosshairRay()` : 화면 중앙에서 월드 레이 시작점/방향 계산.
    - `GetAimTargetPoint()` : 레이 캐스팅으로 히트 지점 또는 최대 거리 위치 반환.
  - 에디터 노출 파라미터 추가
    - `ProjectileClass`, `ProjectileSpeed`, `MuzzleSocketName`.
- **RAC_CPP_Character**
  - 무기 시스템 초기화 흐름 추가
    - `WeaponManager` 서브오브젝트 생성.
    - `TryInitializeWeapon()`로 초기 무기 장착을 안전하게 지연 초기화.
    - `BeginPlay`, `PossessedBy`, `OnRep_PlayerState`에서 초기화 시도.
  - 무기 인벤토리/전환
    - `WeaponInventory`, `CurrentWeaponIndex`, `IsTabHold` 변수 추가.
    - `Tab()` 입력 시 다음 무기로 순환 전환.
  - 사격 입력 처리 개선
    - 능력 활성 우선순위: Charge → Auto → Single.
    - Auto 능력 활성 시, 캐릭터 측 탄약 수동 감소는 제외(자동 사격 능력에서 소비).
    - 디버그 로그 추가.
- **RAC_PlayerController**
  - Tab 입력 액션 추가 및 바인딩
    - `TabAction` 노출.
    - `OnTab()`에서 캐릭터의 `Tab()` 호출.
  - 에임 관련 보조 함수 추가(현재는 사용 여부와 별개로 제공)
    - `GetCrosshairRay()`, `GetAimTargetPoint()`.
- **RAC_HUD_Widget**
  - 탄약 UI 갱신을 위해 `OnAmmoChanged` 델리게이트 바인딩/해제 추가.
- **Config**
  - 입력 리다이렉트: `RAC_PlayerController.TapAction` → `TabAction`.

## 추가/수정 에셋(요약)
- 신규 입력 액션: IA_R_Tab.
- 무기 관련 블루프린트: BP_GA_AutoFire, BP_GE_AmmoCost.
- 캐릭터/컨트롤러/애님 BP 업데이트.

## 메모
- 총구 소켓 이름 기본값: `Muzzle_01`.
- 프로젝타일 발사 속도 기본값: `3000.f`.
