// Fill out your copyright notice in the Description page of Project Settings.


#include "MainHUD.h"
#include "ConstructorHelpers.h"
#include "Engine/Texture2D.h"
#include "Engine/Canvas.h"


AMainHUD::AMainHUD()
{
	HUDTexArray.SetNumUninitialized(2);

	// Crosshair
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairObject(TEXT("/Game/MinecraftContents/Textures/Widgets/Crosshair"));
	if (CrosshairObject.Object != NULL)
	{
		HUDTexArray[static_cast<int>(EHUDType::Crosshair)] = CrosshairObject.Object;
	}
	// QuickSlot
	static ConstructorHelpers::FObjectFinder<UTexture2D> QuickSlotObject(TEXT("/Game/MinecraftContents/Textures/Widgets/widgets"));
	if (QuickSlotObject.Object != NULL)
	{
		HUDTexArray[static_cast<int>(EHUDType::QuickSlot)] = QuickSlotObject.Object;
	}
	BlendModd = ESimpleElementBlendMode::SE_BLEND_Translucent;
}

/*
 * Draw Item
 * @param DrawPosition  Draw Item Position
 * @param UV0			Top Left UV
 * @param UV1			Bottom Right UV
 * @param InScreenSize	Draw Item Size in Screen
 * @param e				HUD Type
 */
void AMainHUD::DrawItem(const FVector2D& DrawPosition, EHUDType e, const FVector2D& DrawItemInScreenSize = FVector2D(0.f, 0.f), const FVector2D& UVCoord0 = FVector2D(0.0f, 0.0f), const FVector2D& UVCoord1 = FVector2D(1.0f, 1.0f))
{
	if (DrawItemInScreenSize.X != 0.f)
	{
		const FVector2D DrawTextureSize = UVCoord1 - UVCoord0;
		const FVector2D TextureSize(HUDTexArray[static_cast<int>(e)]->GetSizeX(), HUDTexArray[static_cast<int>(e)]->GetSizeY());

		FCanvasTileItem TileItem(DrawPosition, HUDTexArray[static_cast<int>(e)]->Resource, DrawItemInScreenSize, UVCoord0 / TextureSize, UVCoord1 / TextureSize, FLinearColor::White);
		TileItem.BlendMode = BlendModd;

		Canvas->DrawItem(TileItem);
	}
	else // full size
	{
		FCanvasTileItem TileItem(DrawPosition, HUDTexArray[static_cast<int>(e)]->Resource, FLinearColor::White);
		TileItem.BlendMode = ESimpleElementBlendMode::SE_BLEND_Translucent;
		
		Canvas->DrawItem(TileItem);
	}
}


void AMainHUD::DrawHUD()
{
	Super::DrawHUD();

	// Crosshair
	const FVector2D CrosshairDrawPosition(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);
	DrawItem(CrosshairDrawPosition, EHUDType::Crosshair);

	// QuickSlot
	const float InScreenSize = 0.5f;
	const FVector2D QSUV1(731.f, 86.f);
	const float QSAspectRatio = QSUV1.Y / QSUV1.X;
	const FVector2D QSScreenSize(Canvas->ClipX * InScreenSize, Canvas->ClipX * InScreenSize * QSAspectRatio);
	const FVector2D QSDrawPosition(Canvas->ClipX * ((1 - InScreenSize) * 0.5f), Canvas->ClipY - QSScreenSize.Y);
	DrawItem(QSDrawPosition, EHUDType::QuickSlot, QSScreenSize, FVector2D(0.0f, 0.0f), QSUV1);

	const FVector2D QSSelectUV0(0.f, 88.f);
	const FVector2D QSSelectUV1(95.f, 184.f);
	//const float QSSelectAspectRatio = ((QSSelectUV1.Y - QSSelectUV0.Y) / (QSSelectUV1.X - QSSelectUV0.X));
	const float QSSelectAspectRatio = ((QSSelectUV1.X - QSSelectUV0.X) / (QSSelectUV1.Y - QSSelectUV0.Y));
	const FVector2D QSSelectScreenSize(QSScreenSize.Y * QSSelectAspectRatio, Canvas->ClipX * InScreenSize * QSAspectRatio);
	FVector2D QSSelectDrawPosition(QSDrawPosition.X, Canvas->ClipY - QSSelectScreenSize.Y);
	DrawItem(QSSelectDrawPosition, EHUDType::QuickSlot, QSSelectScreenSize, QSSelectUV0, QSSelectUV1);
	

}


