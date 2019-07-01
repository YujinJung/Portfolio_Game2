// Fill out your copyright notice in the Description page of Project Settings.


#include "Portfolio_Game2GameModeBase.h"
#include "ConstructorHelpers.h"
#include "CrosshairHUD.h"

APortfolio_Game2GameModeBase::APortfolio_Game2GameModeBase()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawn(TEXT("/Game/Blueprints/BP_Character"));
	if (PlayerPawn.Class != NULL)
	{
		DefaultPawnClass = PlayerPawn.Class;
	}
	static ConstructorHelpers::FClassFinder<ACrosshairHUD> CrosshairHUD(TEXT("/Game/Blueprints/BP_CrosshairHUD"));
	if (CrosshairHUD.Class != NULL)
	{
		HUDClass = CrosshairHUD.Class;
	}
}
