// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BaseHUD.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_GAME2_API ABaseHUD : public AHUD
{
	GENERATED_BODY()
public:
	ABaseHUD();

	void DrawItem(const FVector2D& DrawPosition, UTexture2D* HUDTex, const FVector2D& DrawItemInScreenSize = FVector2D(0.f, 0.f), const FVector2D& UVCoord0 = FVector2D(0.0f, 0.0f), const FVector2D& UVCoord1 = FVector2D(1.0f, 1.0f));
	virtual void DrawHUD() override;

	void SetDrawPosition(const FVector2D& inDrawPosition) { DrawPosition = inDrawPosition; };

private:
	FVector2D DrawPosition;
};

