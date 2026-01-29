// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RAC_HUD_Widget.generated.h"

class ARAC_PlayerState;
class UTextBlock;
class UProgressBar;

UCLASS()
class MYRATCHET_API URAC_HUD_Widget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	// PlayerState 참조
	UPROPERTY()
	ARAC_PlayerState* OwningPlayerState;

	// UI 요소들 (블루프린트에서 바인드)
	UPROPERTY(meta = (BindWidget))
	UTextBlock* HealthText;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* BoltsText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* AmmoText;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* AmmoBar;

	// 델리게이트 바인드
	void BindToAttributeSet();

	// 콜백 함수들
	UFUNCTION()
	void OnHealthChanged(float OldValue, float NewValue);

	UFUNCTION()
	void OnBoltsChanged(float OldValue, float NewValue);

	UFUNCTION()
	void OnAmmoChanged(float OldValue, float NewValue);

	// UI 업데이트 함수
	void UpdateHealthUI(float CurrentHealth, float MaxHealth);
	void UpdateBoltsUI(float CurrentBolts);
	void UpdateAmmoUI(float CurrentAmmo, float MaxAmmo);
};
