// Fill out your copyright notice in the Description page of Project Settings.


#include "TechActor/JumPad.h"

// Sets default values
AJumPad::AJumPad()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AJumPad::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AJumPad::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

