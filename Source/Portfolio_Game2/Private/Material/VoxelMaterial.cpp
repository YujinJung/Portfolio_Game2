// Fill out your copyright notice in the Description page of Project Settings.


#include "VoxelMaterial.h"
#include "ConstructorHelpers.h"
#include "Engine/Texture2D.h"

UVoxelMaterial::UVoxelMaterial()
{
	static ConstructorHelpers::FObjectFinder<UTexture2D> GrassTopTex(TEXT("/Game/MinecraftContents/Textures/Voxels/T_Grass_Top"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> GrassBotTex(TEXT("/Game/MinecraftContents/Textures/Voxels/T_Dirt"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> GrassSideTex(TEXT("/Game/MinecraftContents/Textures/Voxels/T_Grass_Side"));
	//checkf((GrassTopTex.Object == NULL) || (GrassBotTex.Object == NULL) || (GrassSideTex.Object == NULL), TEXT("Miss Grass Voxel Texture"));

	
	
}


