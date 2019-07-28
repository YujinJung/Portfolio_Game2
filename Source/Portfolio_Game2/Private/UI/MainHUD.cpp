// Fill out your copyright notice in the Description page of Project Settings.

#include "MainHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "ConstructorHelpers.h"
#include "UserWidget.h"


AMainHUD::AMainHUD()
{
	MainHUDTextureArray.SetNumUninitialized(EHUDType::Count);

	// Crosshair
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairObject(TEXT("/Game/MinecraftContents/Textures/Widgets/Crosshair"));
	if (CrosshairObject.Object != NULL)
	{
		MainHUDTextureArray[EHUDType::Crosshair] = CrosshairObject.Object;
	}
}

/*
 * Draw Item
 * @param DrawPosition			Draw Item Position
 * @param e						HUD Type
 * @param DrawItemInScreenSize  Draw Item Size in Screen
 * @param UV0					Top Left UV
 * @param UV1					Bottom Right UV
 */
void AMainHUD::DrawItem(const FVector2D& DrawPosition, EHUDType e, const FVector2D& DrawItemInScreenSize, const FVector2D& UVCoord0, const FVector2D& UVCoord1)
{
	if (DrawItemInScreenSize.X != 0.f)
	{
		const FVector2D DrawTextureSize = UVCoord1 - UVCoord0;
		const FVector2D TextureSize(MainHUDTextureArray[e]->GetSizeX(), MainHUDTextureArray[e]->GetSizeY());

		FCanvasTileItem TileItem(DrawPosition, MainHUDTextureArray[e]->Resource, DrawItemInScreenSize, UVCoord0 / TextureSize, UVCoord1 / TextureSize, FLinearColor::White);
		TileItem.BlendMode = ESimpleElementBlendMode::SE_BLEND_Translucent;

		Canvas->DrawItem(TileItem);
	}
	else // full size
	{
		FCanvasTileItem TileItem(DrawPosition, MainHUDTextureArray[e]->Resource, FLinearColor::White);
		TileItem.BlendMode = ESimpleElementBlendMode::SE_BLEND_Translucent;

		Canvas->DrawItem(TileItem);
	}
}

void AMainHUD::DrawStaticItem()
{
	// Crosshair
	const FVector2D CrosshairDrawPosition(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);
	DrawItem(CrosshairDrawPosition, EHUDType::Crosshair);
}

void AMainHUD::DrawHUD()
{
	Super::DrawHUD();

	DrawStaticItem();
}

void AMainHUD::BeginPlay()
{
	Super::BeginPlay();
}
