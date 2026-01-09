// Fill out your copyright notice in the Description page of Project Settings.


#include "RAC_Character.h"

// Sets default values
ARAC_Character::ARAC_Character()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ARAC_Character::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame

/*
void ARAC_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
*/


// Called to bind functionality to input
void ARAC_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

