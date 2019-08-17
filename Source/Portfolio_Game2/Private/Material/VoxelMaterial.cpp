// Fill out your copyright notice in the Description page of Project Settings.


#include "VoxelMaterial.h"
#include "ConstructorHelpers.h"
#include "Engine/Texture2D.h"

UVoxelMaterial::UVoxelMaterial()
{
	static ConstructorHelpers::FObjectFinder<UTexture2D> DestroyedVoxelTex(TEXT("/Game/MinecraftContents/Textures/Voxels/T_DestroyedVoxel"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> DestroyedVoxelMaskTex(TEXT("/Game/MinecraftContents/Textures/Voxels/T_DestroyedVoxelMask"));
	
}

void UVoxelMaterial::RefreshMaterial()
{
	
}
