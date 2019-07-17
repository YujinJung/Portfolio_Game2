// Fill out your copyright notice in the Description page of Project Settings.


#include "CrosshairHUD.h"
#include "ConstructorHelpers.h"
#include "Engine/Texture2D.h"
#include "Engine/Canvas.h"

ACrosshairHUD::ACrosshairHUD()
{
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairObject(TEXT("/Game/Widgets/Textures/Crosshair"));

	if (CrosshairObject.Object != NULL)
	{
		CrosshairTex = CrosshairObject.Object;
	}
}

void ACrosshairHUD::DrawHUD()
{
	Super::DrawHUD();

	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);
	//const FVector2D CrosshairDrawPosition(Center.X - 5.f, Center.Y - 5.f);
	const FVector2D CrosshairDrawPosition(Center.X, Center.Y);

	FCanvasTileItem TileItem(CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
	TileItem.BlendMode = ESimpleElementBlendMode::SE_BLEND_Translucent;

	Canvas->DrawItem(TileItem);
}
