// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CrosshairHUD.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_GAME2_API ACrosshairHUD : public AHUD
{
	GENERATED_BODY()

public:
	ACrosshairHUD();

private:
	virtual void DrawHUD();
	class UTexture2D* CrosshairTex;

};
