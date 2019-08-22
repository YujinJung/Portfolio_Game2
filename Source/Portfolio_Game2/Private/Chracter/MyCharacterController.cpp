// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacterController.h"
#include "MyCharacter.h"

void AMyCharacterController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	if (InPawn)
	{
		auto PossessedCharacter = Cast<AMyCharacter>(InPawn);
		if (!ensure(PossessedCharacter))
		{
			return;
		}
	}
}

void AMyCharacterController::BeginPlay()
{
	Super::BeginPlay();
}

void AMyCharacterController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
