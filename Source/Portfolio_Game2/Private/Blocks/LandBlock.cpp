// Fill out your copyright notice in the Description page of Project Settings.


#include "LandBlock.h"
#include "Components/StaticMeshComponent.h"
#include "ConstructorHelpers.h"
#include "TimerManager.h"

ALandBlock::ALandBlock()
	: ABlock(TEXT("/Game/MinecraftContents/Materials/Block/M_Dirt")), GrassTime(0.f)
{
	State = LandCubeState::Grass;
	PrimaryActorTick.bCanEverTick = true;
}

void ALandBlock::BeginPlay()
{
	Super::BeginPlay();
	GrowTimer();
}

void ALandBlock::SetGrass()
{
	SetMaterial(BlockSurface::Top, TEXT("/Game/MinecraftContents/Materials/Block/M_Grass_Path_Top"));
	SetMaterial(BlockSurface::Side, TEXT("/Game/MinecraftContents/Materials/Block/M_Grass_Path_Side"));
	GrassTime = 10.f;
}

void ALandBlock::GrowTimer()
{
	GetWorldTimerManager().SetTimer(GrassTimerHandle, this, &ALandBlock::GrowGrass, 1.f, true, 10.f);
}

void ALandBlock::GrowGrass()
{
	if (State == LandCubeState::Dirt)
	{
		GrassTime = 0.f;
		SetMaterial(BlockSurface::Top, TEXT("/Game/MinecraftContents/Materials/Block/M_Dirt"));
		SetMaterial(BlockSurface::Side, TEXT("/Game/MinecraftContents/Materials/Block/M_Dirt"));

		return;
	}

	GrassTime++;
	if (GrassTime > 10.f)
	{
		GetWorldTimerManager().ClearTimer(GrassTimerHandle);
	}
	else if (GrassTime > 8.f)
	{
		SetMaterial(BlockSurface::Top, TEXT("/Game/MinecraftContents/Materials/Block/M_Grass_Path_Top"));
		SetMaterial(BlockSurface::Side, TEXT("/Game/MinecraftContents/Materials/Block/M_Grass_Path_Side"));
	}
	else if (GrassTime > 6.f)
	{
		SetMaterial(BlockSurface::Top, TEXT("/Game/MinecraftContents/Materials/Block/M_Dirt2"));
	}
	else if (GrassTime > 4.f)
	{
		SetMaterial(BlockSurface::Top, TEXT("/Game/MinecraftContents/Materials/Block/M_Dirt1"));
	}
}

void ALandBlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (State == LandCubeState::Dirt)
	{
		GrassTime = 0.f;
		SetMaterial(BlockSurface::Top, TEXT("/Game/MinecraftContents/Materials/Block/M_Dirt"));
		SetMaterial(BlockSurface::Side, TEXT("/Game/MinecraftContents/Materials/Block/M_Dirt"));

		return;
	}
}
