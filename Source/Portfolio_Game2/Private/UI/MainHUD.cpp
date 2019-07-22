// Fill out your copyright notice in the Description page of Project Settings.

#include "MainHUD.h"
#include "MainUI.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "ConstructorHelpers.h"
#include "UserWidget.h"


AMainHUD::AMainHUD()
{
	Initialize();
}

void AMainHUD::Initialize()
{
	MainHUDTextureArray.SetNumUninitialized(EHUDType::Count);

	// Crosshair
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairObject(TEXT("/Game/MinecraftContents/Textures/Widgets/Crosshair"));
	if (CrosshairObject.Object != NULL)
	{
		MainHUDTextureArray[EHUDType::Crosshair] = CrosshairObject.Object;
	}
	// QuickSlot
	static ConstructorHelpers::FObjectFinder<UTexture2D> QuickSlotObject(TEXT("/Game/MinecraftContents/Textures/Widgets/widgets"));
	if (QuickSlotObject.Object != NULL)
	{
		MainHUDTextureArray[EHUDType::QuickSlot] = QuickSlotObject.Object;
	}
}

/*
 * Draw Item
 * @param DrawPosition  Draw Item Position
 * @param UV0			Top Left UV
 * @param UV1			Bottom Right UV
 * @param InScreenSize	Draw Item Size in Screen
 * @param e				HUD Type
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

	//// QuickSlot
	//const float InScreenSize = 0.5f;
	//const FVector2D QSUV1(731.f, 86.f);
	//const float QSAspectRatio = QSUV1.Y / QSUV1.X;
	//const float QSScreenSizeX = Canvas->ClipX * InScreenSize;

	//FVector2D QSScreenSize(QSScreenSizeX, QSScreenSizeX * QSAspectRatio);
	//FVector2D QSDrawPosition(Canvas->ClipX * ((1 - InScreenSize) * 0.5f), Canvas->ClipY - QSScreenSize.Y);

	//DrawItem(QSDrawPosition, EHUDType::QuickSlot, QSScreenSize, FVector2D(0.0f, 0.0f), QSUV1);
}

// QuickSlot Select
/*void AMainHUD::DrawQSSelect(int QuickSlotNum)
{
	const float InScreenSize = 0.5f;
	const FVector2D QSUV1(731.f, 86.f);
	const float QSAspectRatio = QSUV1.Y / QSUV1.X;
	const float QSScreenSizeX = Canvas->ClipX * InScreenSize;

	const FVector2D QSSelectUV0(0.f, 88.f);
	const FVector2D QSSelectUV1(95.f, 184.f);
	const float QSSelectAspectRatio = ((QSSelectUV1.X - QSSelectUV0.X) / (QSSelectUV1.Y - QSSelectUV0.Y));

	FVector2D QSSelectScreenSize(QSScreenSizeX * QSAspectRatio * QSSelectAspectRatio, QSScreenSizeX * QSAspectRatio);
	FVector2D QSSelectDrawPosition(Canvas->ClipX * ((1 - InScreenSize) * 0.5f) + (QSScreenSizeX / 9 * QuickSlotNum) , Canvas->ClipY - QSSelectScreenSize.Y);

	DrawItem(QSSelectDrawPosition, EHUDType::QuickSlot, QSSelectScreenSize, QSSelectUV0, QSSelectUV1);
}*/

void AMainHUD::DrawHUD()
{
	Super::DrawHUD();

	DrawStaticItem();

}

void AMainHUD::BeginPlay()
{
	Super::BeginPlay();

	//CurrentWidget = CreateWidget<UMainUI>(GetWorld(), UMainUI::StaticClass());

	//if (CurrentWidget)
	//{
	//	CurrentWidget->AddToViewport();
	//}
}
