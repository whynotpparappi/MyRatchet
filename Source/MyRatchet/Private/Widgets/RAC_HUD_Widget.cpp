// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/RAC_HUD_Widget.h"
#include "Frameworks/RAC_PlayerState.h"
#include "Characters/RAC_AttributeSet.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

void URAC_HUD_Widget::NativeConstruct()
{
	Super::NativeConstruct();

	// 소유 플레이어의 PlayerState 가져오기
	APlayerController* PlayerController = GetOwningPlayer();
	if (PlayerController)
	{
		OwningPlayerState = Cast<ARAC_PlayerState>(PlayerController->PlayerState);
		if (OwningPlayerState)
		{
			BindToAttributeSet();
		}
	}
}

void URAC_HUD_Widget::NativeDestruct()
{
	// 델리게이트 언바인드
	if (OwningPlayerState && OwningPlayerState->AttributeSet)
	{
		OwningPlayerState->AttributeSet->OnHealthChanged.RemoveDynamic(this, &URAC_HUD_Widget::OnHealthChanged);
		OwningPlayerState->AttributeSet->OnBoltsChanged.RemoveDynamic(this, &URAC_HUD_Widget::OnBoltsChanged);
		OwningPlayerState->AttributeSet->OnAmmoChanged.RemoveDynamic(this, &URAC_HUD_Widget::OnAmmoChanged);
	}

	Super::NativeDestruct();
}

void URAC_HUD_Widget::BindToAttributeSet()
{
	if (!OwningPlayerState || !OwningPlayerState->AttributeSet)
	{
		return;
	}

	URAC_AttributeSet* AttributeSet = OwningPlayerState->AttributeSet;

	// 델리게이트 바인드
	AttributeSet->OnHealthChanged.AddDynamic(this, &URAC_HUD_Widget::OnHealthChanged);
	AttributeSet->OnBoltsChanged.AddDynamic(this, &URAC_HUD_Widget::OnBoltsChanged);
	AttributeSet->OnAmmoChanged.AddDynamic(this, &URAC_HUD_Widget::OnAmmoChanged);

	// 초기 UI 업데이트
	UpdateHealthUI(AttributeSet->GetHealth(), AttributeSet->GetMaxHealth());
	UpdateBoltsUI(AttributeSet->GetBolts());
	UpdateAmmoUI(AttributeSet->GetAmmo(), AttributeSet->GetMaxAmmo());
}

void URAC_HUD_Widget::OnHealthChanged(float OldValue, float NewValue)
{
	if (OwningPlayerState && OwningPlayerState->AttributeSet)
	{
		float MaxHealth = OwningPlayerState->AttributeSet->GetMaxHealth();
		UpdateHealthUI(NewValue, MaxHealth);
	}
}

void URAC_HUD_Widget::OnBoltsChanged(float OldValue, float NewValue)
{
	UpdateBoltsUI(NewValue);
}

void URAC_HUD_Widget::OnAmmoChanged(float OldValue, float NewValue)
{
	if (OwningPlayerState && OwningPlayerState->AttributeSet)
	{
		float MaxAmmo = OwningPlayerState->AttributeSet->GetMaxAmmo();
		UpdateAmmoUI(NewValue, MaxAmmo);
	}
}

void URAC_HUD_Widget::UpdateHealthUI(float CurrentHealth, float MaxHealth)
{
	if (HealthText)
	{
		HealthText->SetText(FText::AsNumber(FMath::RoundToInt(CurrentHealth)));
	}

	if (HealthBar && MaxHealth > 0.f)
	{
		HealthBar->SetPercent(CurrentHealth / MaxHealth);
	}
}

void URAC_HUD_Widget::UpdateBoltsUI(float CurrentBolts)
{
	if (BoltsText)
	{
		BoltsText->SetText(FText::AsNumber(FMath::RoundToInt(CurrentBolts)));
	}
}

void URAC_HUD_Widget::UpdateAmmoUI(float CurrentAmmo, float MaxAmmo)
{
	if (AmmoText)
	{
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
