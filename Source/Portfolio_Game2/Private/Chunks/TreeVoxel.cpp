// Fill out your copyright notice in the Description page of Project Settings.


#include "TreeVoxel.h"
#include "Materials/Material.h"

// Sets default values
ATreeVoxel::ATreeVoxel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	AddVoxelMaterial(TEXT("M_Log"));
	AddVoxelMaterial(TEXT("M_Leaves"));
	AddVoxelMaterial(TEXT("M_Leaves_Far"));
}

// Called when the game starts or when spawned
void ATreeVoxel::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATreeVoxel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATreeVoxel::AddVoxelMaterial(FString MaterialName)
{
	FString MaterialPath = TEXT("/Game/MinecraftContents/Materials/Voxels/");
	MaterialPath.Append(MaterialName);
	UMaterial* VoxelMaterial = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, *MaterialPath));

	if (VoxelMaterial)
	{
		VoxelMaterials.Add(VoxelMaterial);
	}
}

void ATreeVoxel::GenerateVoxel()
{
}

