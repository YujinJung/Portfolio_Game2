// Fill out your copyright notice in the Description page of Project Settings.


#include "MainUI.h"
#include "ConstructorHelpers.h"

UMainUI::UMainUI(const FObjectInitializer& ObjectInitializer)
	: UUserWidget(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UTexture2D> QuickSlotObject(TEXT("/Game/MinecraftContents/Textures/Widgets/widgets"));

	if (QuickSlotObject.Object != NULL)
	{
		QuickSlotTex = QuickSlotObject.Object;
	}
}

 
