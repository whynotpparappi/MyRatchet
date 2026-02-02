// Fill out your copyright notice in the Description page of Project Settings.

#include "Effects/GE_AmmoCost.h"

#include "Characters/RAC_AttributeSet.h"

UGE_AmmoCost::UGE_AmmoCost()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FGameplayModifierInfo Modifier;
	Modifier.Attribute = URAC_AttributeSet::GetAmmoAttribute();
	Modifier.ModifierOp = EGameplayModOp::Additive;
	Modifier.ModifierMagnitude = FScalableFloat(-1.0f);
	Modifiers.Add(Modifier);
}
