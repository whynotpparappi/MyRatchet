// Fill out your copyright notice in the Description page of Project Settings.


#include "Gamemode/RAC_Cpp_GameMode.h"
#include "characters/RAC_CPP_Character.h"
#include "UObject/ConstructorHelpers.h"


ARAC_Cpp_GameMode::ARAC_Cpp_GameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBpClass(TEXT("/Game/_M_RAC/Character/RAC_PlayerCharacter.RAC_PlayerCharacter_C"));
	if (PlayerPawnBpClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBpClass.Class;
	}
}
