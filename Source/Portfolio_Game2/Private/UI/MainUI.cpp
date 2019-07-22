// Fill out your copyright notice in the Description page of Project Settings.


#include "MainUI.h"
#include "ConstructorHelpers.h"
#include "Image.h"
#include "Engine/Texture2D.h"
#include "PaperSprite.h"
#include "WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
	
UMainUI::UMainUI(const FObjectInitializer& ObjectInitializer)
	: UUserWidget(ObjectInitializer)
{
}

void UMainUI::NativeConstruct()
{
	Super::NativeConstruct();

	//QuickSlotFocusImg = this->WidgetTree->FindWidget<UImage>("QSSelect");
	//if (QSSelectImg != NULL)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Star"));
	//}

}

//void UMainUI::UpdateQuickSlotFocus()
//{
	// create image and set its position
	//QuickSlotFocusImg = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("QuickSlotFocus"));
	//static ConstructorHelpers::FObjectFinder<UPaperSprite> QuickSlotFocusObject(TEXT("/Game/MinecraftContents/Textures/Widgets/QuickSlotFocus"));
	//if (QuickSlotFocusObject.Object != NULL)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Load Success"));
	//	const FSlateAtlasData SpriteAtlasData = QuickSlotFocusObject.Object->GetSlateAtlasData();
	//	const FVector2D SpriteSize = SpriteAtlasData.GetSourceDimensions();
	//	FSlateBrush Brush;
	//	Brush.SetResourceObject(QuickSlotFocusObject.Object);
	//	Brush.ImageSize = FVector2D(SpriteSize.X, SpriteSize.Y);
	//	QuickSlotFocusImg->SetBrush(Brush);
	//	QuickSlotFocusImg->SetColorAndOpacity(FLinearColor(0, 0, 0, 0));
	//}

	//// add it to child of root
	//UPanelWidget* root = Cast<UPanelWidget>(GetRootWidget());
	//UPanelSlot* slot = root->AddChild(QuickSlotFocusImg);
	//UCanvasPanelSlot* canvasSlot = Cast<UCanvasPanelSlot>(slot);

	//// set position and stuff
	//canvasSlot->SetAnchors(FAnchors(0.5f, 0.5f));
	//canvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));
//}

