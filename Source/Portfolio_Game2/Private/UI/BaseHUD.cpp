#include "BaseHUD.h"
#include "ConstructorHelpers.h"
#include "Engine/Texture2D.h"
#include "Engine/Canvas.h"


ABaseHUD::ABaseHUD()
{

}

/*
 * Draw Item
 * @param DrawPosition  Draw Item Position
 * @param UV0			Top Left UV
 * @param UV1			Bottom Right UV
 * @param InScreenSize	Draw Item Size in Screen
 * @param e				HUD Type
 */
void ABaseHUD::DrawItem(const FVector2D& DrawPosition, UTexture2D* HUDTex, const FVector2D& DrawItemInScreenSize, const FVector2D& UVCoord0, const FVector2D& UVCoord1)
//void AMainHUD::DrawItem(const FVector2D& DrawPosition, UTexture2D* HUDTex, const FVector2D& DrawItemInScreenSize = FVector2D(0.f, 0.f), const FVector2D& UVCoord0 = FVector2D(0.0f, 0.0f), const FVector2D& UVCoord1 = FVector2D(1.0f, 1.0f))
{
	if (DrawItemInScreenSize.X != 0.f)
	{
		const FVector2D DrawTextureSize = UVCoord1 - UVCoord0;
		const FVector2D TextureSize(HUDTex->GetSizeX(), HUDTex->GetSizeY());

		FCanvasTileItem TileItem(DrawPosition, HUDTex->Resource, DrawItemInScreenSize, UVCoord0 / TextureSize, UVCoord1 / TextureSize, FLinearColor::White);
		TileItem.BlendMode = ESimpleElementBlendMode::SE_BLEND_Translucent;

		Canvas->DrawItem(TileItem);
	}
	else // full size
	{
		FCanvasTileItem TileItem(DrawPosition, HUDTex->Resource, FLinearColor::White);
		TileItem.BlendMode = ESimpleElementBlendMode::SE_BLEND_Translucent;

		Canvas->DrawItem(TileItem);
	}
}


void ABaseHUD::DrawHUD()
{
	Super::DrawHUD();

}

