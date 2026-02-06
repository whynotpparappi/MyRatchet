// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "YourProjectTypes.generated.h" // 반드시 포함되어야 함

// 1. 적의 타입을 정의하는 Enum
UENUM(BlueprintType)
enum class EEnemyType : uint8
{
	Melee      UMETA(DisplayName = "Melee"),      // 근거리 적
	Ranged     UMETA(DisplayName = "Ranged"),     // 원거리 적
	Boss       UMETA(DisplayName = "Boss")        // 보스
};

// 2. (추가 팁) 나중에 GAS에서 사용할 능력 종류(Action State) 등도 여기에 모으면 편합니다.
UENUM(BlueprintType)
enum class EAIActionState : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	Chasing UMETA(DisplayName = "Chasing"),
	Attacking UMETA(DisplayName = "Attacking"),
	Dead UMETA(DisplayName = "Dead")
};
