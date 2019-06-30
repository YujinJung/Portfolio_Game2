// Fill out your copyright notice in the Description page of Project Settings.


#include "Portfolio_Game2GameModeBase.h"
#include "ConstructorHelpers.h"

APortfolio_Game2GameModeBase::APortfolio_Game2GameModeBase()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawn(TEXT("/Game/Blueprints/MyCharacter"));

	if (PlayerPawn.Class != NULL)
	{
		DefaultPawnClass = PlayerPawn.Class;
	}

}
