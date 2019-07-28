// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MainHUD.generated.h"

/**
 * 
 */
enum EHUDType {
	Crosshair,
	Count
};

UCLASS()
class PORTFOLIO_GAME2_API AMainHUD : public AHUD
{
	GENERATED_BODY()

public:
	AMainHUD();

	void DrawItem(const FVector2D& DrawPosition, EHUDType e, const FVector2D& DrawItemInScreenSize = FVector2D(0.f, 0.f), const FVector2D& UVCoord0 = FVector2D(0.0f, 0.0f), const FVector2D& UVCoord1 = FVector2D(1.0f, 1.0f));

	void DrawStaticItem();

	virtual void DrawHUD() override;

	virtual void BeginPlay() override;


private:
	TArray<class UTexture2D*> MainHUDTextureArray;

};
