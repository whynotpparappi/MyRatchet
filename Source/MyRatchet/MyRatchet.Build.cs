// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MyRatchet : ModuleRules
{
	public MyRatchet(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", 
																	"CoreUObject",
																	"Engine",
																	"InputCore",
																	"EnhancedInput",
																	"AIModule",				// BTTask의 필수라 함
																	"GameplayAbilities",	// 게임플레이 어빌리티 추가
																	"GameplayTags",			// 게임플레이 태그스
																	"GameplayTasks"			// 게임플레이 태스크
		});
	}
}
