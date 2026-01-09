// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyRatchetGameMode.h"
#include "MyRatchetCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMyRatchetGameMode::AMyRatchetGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
