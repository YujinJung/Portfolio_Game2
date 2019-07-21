// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MainHUD.generated.h"

/**
 * 
 */

enum class EHUDType : uint8 {
	Crosshair,
	QuickSlot,
};

UCLASS()
class PORTFOLIO_GAME2_API AMainHUD : public AHUD
{
	GENERATED_BODY()

public:
	AMainHUD();

private:
	void DrawItem(const FVector2D& DrawPosition, EHUDType e, const FVector2D& DrawItemInScreenSize, const FVector2D& UVCoord0, const FVector2D& UVCoord1);
	virtual void DrawHUD() override;
	
private:
	TArray<class UTexture2D*> HUDTexArray;
	ESimpleElementBlendMode BlendModd;
};
