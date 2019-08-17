// Fill out your copyright notice in the Description page of Project Settings.

#include "VoxelChunk.h"
#include "SimplexNoiseBPLibrary.h"
#include "ProceduralMeshComponent.h"
#include "Materials/Material.h"

const int32 bTriangles[] = { 2, 1, 0, 0, 3, 2 }; // draw triangle order
const FVector2D bUVs[] = { FVector2D(0.000000, 0.000000), FVector2D(0.000000, 1.000000), FVector2D(1.000000, 1.000000), FVector2D(1.000000, 0.000000) };
// top
const FVector bNormals0[] = { FVector(0, 0, 1), FVector(0, 0, 1), FVector(0, 0, 1), FVector(0, 0, 1) };

// bot
const FVector bNormals1[] = { FVector(0, 0, -1), FVector(0, 0, -1), FVector(0, 0, -1), FVector(0, 0, -1) };

// right
const FVector bNormals2[] = { FVector(0, 1, 0), FVector(0, 1, 0), FVector(0, 1, 0), FVector(0, 1, 0) };

// left
const FVector bNormals3[] = { FVector(0, -1, 0), FVector(0, -1, 0), FVector(0, -1, 0), FVector(0, -1, 0) };

// front
const FVector bNormals4[] = { FVector(1, 0, 0), FVector(1, 0, 0), FVector(1, 0, 0), FVector(1, 0, 0) };

// back
const FVector bNormals5[] = { FVector(-1, 0, 0), FVector(-1, 0, 0), FVector(-1, 0, 0), FVector(-1, 0, 0) };
const FVector bMask[] = { FVector(0.000000, 0.000000, 1.000000),FVector(0.000000, 0.000000, -1.000000),FVector(0.000000, 1.000000, 0.000000),FVector(0.000000, -1.000000, 0.000000), FVector(1.000000, 0.000000, 0.000000), FVector(-1.000000, 0.000000, 0.000000) };


struct FVoxelChunkSection
{
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UV;
	TArray<FColor> VertexColors;
	TArray<FProcMeshTangent> Tangents;

	int32 elementID = 0;
};

// Sets default values
AVoxelChunk::AVoxelChunk()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	chunkZSize = 50;
	chunkXYSize = 16;
	chunkXYSizeX2 = chunkXYSize * chunkXYSize;
	chunkTotalSize = chunkXYSize * chunkXYSize * chunkZSize;

	FString name = "Voxel_" + FString::FromInt(chunkXIndex) + "_" + FString::FromInt(chunkYIndex);
	VoxelMeshComponent = NewObject<UProceduralMeshComponent>(this, *name);
	VoxelMeshComponent->RegisterComponent();

	RootComponent = VoxelMeshComponent;

	voxelSize = 100.f;
	voxelHalfSize = voxelSize / 2;
	DestroyStage = 0.f;
	CurrentDestroyVoxelIndex = -1;

	MaxDestroyValue = 80.f;
	DestroySpeed = 1.f;

	/*{
		FString MaterialPath(TEXT("/Game/MinecraftContents/Materials/Voxels/M_Voxel"));
		VoxelMaterials = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, *MaterialPath));

		if (VoxelMaterials)
		{
			VoxelMeshComponent->SetMaterial(0, VoxelMaterials);
		}
	}*/
	SetVoxelMaterial(TEXT("/Game/MinecraftContents/Materials/Voxels/M_Dirt"));
	SetVoxelMaterial(TEXT("/Game/MinecraftContents/Materials/Voxels/M_Grass"));
	SetVoxelMaterial(TEXT("/Game/MinecraftContents/Materials/Voxels/M_Sand"));
}

void AVoxelChunk::SetVoxelMaterial(FString MaterialPath)
{
	UMaterial* VoxelMaterial = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, *MaterialPath));

	if (VoxelMaterial)
	{
		VoxelMaterials.Add(VoxelMaterial);
	}
}

float AVoxelChunk::CalcDensity(float x, float y, float z)
{
	const float cliffScale = 7.f;
	const float noiseScale = 5.f;
	const float offset = 5.f;
	float noise = USimplexNoiseBPLibrary::SimplexNoise3D(x * 0.02f, y * 0.02f, z * 0.04f);

	float cliff = (noise * 0.5f + 0.5f) * cliffScale;
	float density = (noise + cliff) * noiseScale + offset - z;
	//UE_LOG(LogTemp, Warning, TEXT("Noise : %.4f"), noise);
	return density;
}

void AVoxelChunk::GenerateChunk(const FVector& ChunkLocation)
{
	chunkElements.SetNumUninitialized(chunkTotalSize);

	for (int32 x = 0; x < chunkXYSize; ++x)
	{
		for (int32 y = 0; y < chunkXYSize; ++y)
		{
			bool isTop = true; // check top 
			for (int32 z = chunkZSize - 1; z >= 0; --z)
			{
				int32 index = x + (y * chunkXYSize) + (z * chunkXYSizeX2);

				float density = CalcDensity((ChunkLocation.X * chunkXYSize) + x, (ChunkLocation.Y * chunkXYSize) + y, z);

				if (density >= 0.f)
				{
					if (isTop)
					{
						chunkElements[index] = 2;
						isTop = false;
					}
					else
					{
						chunkElements[index] = 1;
					}
				}
				else
				{
					chunkElements[index] = 0;
				}
			}
		}
	}

	UpdateMesh();
}
void AVoxelChunk::UpdateMesh()
{
	TArray<FVoxelChunkSection> ChunkSection;
	ChunkSection.SetNum(VoxelMaterials.Num());

	for (int32 x = 0; x < chunkXYSize; ++x)
	{
		for (int32 y = 0; y < chunkXYSize; ++y)
		{
			for (int32 z = 0; z < chunkZSize; ++z)
			{
				int32 index = x + (y * chunkXYSize) + (z * chunkXYSizeX2);
				/* TODO : Make Destroy Stage Variable */
				int32 VoxelMeshIndex = chunkElements[index] - (static_cast<float>(chunkElements[index] / 100) * 100);
				VoxelMeshIndex--;

				if (VoxelMeshIndex >= 0)
				{
					int32& elementID = ChunkSection[VoxelMeshIndex].elementID;
					TArray<FVector>& Vertices = ChunkSection[VoxelMeshIndex].Vertices;
					TArray<int32>& Triangles = ChunkSection[VoxelMeshIndex].Triangles;
					TArray<FVector>& Normals = ChunkSection[VoxelMeshIndex].Normals;
					TArray<FVector2D>& UV = ChunkSection[VoxelMeshIndex].UV;
					TArray<FColor>& VertexColors = ChunkSection[VoxelMeshIndex].VertexColors;
					TArray<FProcMeshTangent>& Tangents = ChunkSection[VoxelMeshIndex].Tangents;


					int triangleVerticeNum = 0;
					for (int i = 0; i < 6; ++i)
					{
						// next or previous Voxel for each side
						int newIndex = index + bMask[i].X + (bMask[i].Y * chunkXYSize) + (bMask[i].Z * chunkXYSizeX2);

						bool flag = true;

						FVector newIndexVector(x, y, z);
						newIndexVector += bMask[i];

						if ((newIndexVector.X < chunkXYSize) && (newIndexVector.X >= 0) &&
							(newIndexVector.Y < chunkXYSize) && (newIndexVector.Y >= 0) &&
							(newIndexVector.Z < chunkZSize) && (newIndexVector.Z >= 0))
						{
							if (newIndex < chunkElements.Num() && newIndex >= 0)
								if (chunkElements[newIndex] > 0)
									flag = false;
						}

						if (flag)
						{
							for (int j = 0; j < 6; ++j)
							{
								Triangles.Add(bTriangles[j] + triangleVerticeNum + elementID);
							}
							triangleVerticeNum += 4;


							switch (i)
							{
							case 0:
							{
								Vertices.Add(FVector(-voxelHalfSize + (x * voxelSize), voxelHalfSize + (y * voxelSize), voxelHalfSize + (z * voxelSize)));
								Vertices.Add(FVector(-voxelHalfSize + (x * voxelSize), -voxelHalfSize + (y * voxelSize), voxelHalfSize + (z * voxelSize)));
								Vertices.Add(FVector(voxelHalfSize + (x * voxelSize), -voxelHalfSize + (y * voxelSize), voxelHalfSize + (z * voxelSize)));
								Vertices.Add(FVector(voxelHalfSize + (x * voxelSize), voxelHalfSize + (y * voxelSize), voxelHalfSize + (z * voxelSize)));

								Normals.Append(bNormals0, ARRAY_COUNT(bNormals0));
								break;
							}
							case 1:
							{
								Vertices.Add(FVector(voxelHalfSize + (x * voxelSize), -voxelHalfSize + (y * voxelSize), -voxelHalfSize + (z * voxelSize)));
								Vertices.Add(FVector(-voxelHalfSize + (x * voxelSize), -voxelHalfSize + (y * voxelSize), -voxelHalfSize + (z * voxelSize)));
								Vertices.Add(FVector(-voxelHalfSize + (x * voxelSize), voxelHalfSize + (y * voxelSize), -voxelHalfSize + (z * voxelSize)));
								Vertices.Add(FVector(voxelHalfSize + (x * voxelSize), voxelHalfSize + (y * voxelSize), -voxelHalfSize + (z * voxelSize)));

								Normals.Append(bNormals1, ARRAY_COUNT(bNormals1));
								break;
							}
							case 2:
							{
								Vertices.Add(FVector(voxelHalfSize + (x * voxelSize), voxelHalfSize + (y * voxelSize), voxelHalfSize + (z * voxelSize)));
								Vertices.Add(FVector(voxelHalfSize + (x * voxelSize), voxelHalfSize + (y * voxelSize), -voxelHalfSize + (z * voxelSize)));
								Vertices.Add(FVector(-voxelHalfSize + (x * voxelSize), voxelHalfSize + (y * voxelSize), -voxelHalfSize + (z * voxelSize)));
								Vertices.Add(FVector(-voxelHalfSize + (x * voxelSize), voxelHalfSize + (y * voxelSize), voxelHalfSize + (z * voxelSize)));

								Normals.Append(bNormals2, ARRAY_COUNT(bNormals2));
								break;
							}
							case 3:
							{
								Vertices.Add(FVector(-voxelHalfSize + (x * voxelSize), -voxelHalfSize + (y * voxelSize), voxelHalfSize + (z * voxelSize)));
								Vertices.Add(FVector(-voxelHalfSize + (x * voxelSize), -voxelHalfSize + (y * voxelSize), -voxelHalfSize + (z * voxelSize)));
								Vertices.Add(FVector(voxelHalfSize + (x * voxelSize), -voxelHalfSize + (y * voxelSize), -voxelHalfSize + (z * voxelSize)));
								Vertices.Add(FVector(voxelHalfSize + (x * voxelSize), -voxelHalfSize + (y * voxelSize), voxelHalfSize + (z * voxelSize)));

								Normals.Append(bNormals3, ARRAY_COUNT(bNormals3));
								break;
							}
							case 4:
							{
								Vertices.Add(FVector(voxelHalfSize + (x * voxelSize), -voxelHalfSize + (y * voxelSize), voxelHalfSize + (z * voxelSize)));
								Vertices.Add(FVector(voxelHalfSize + (x * voxelSize), -voxelHalfSize + (y * voxelSize), -voxelHalfSize + (z * voxelSize)));
								Vertices.Add(FVector(voxelHalfSize + (x * voxelSize), voxelHalfSize + (y * voxelSize), -voxelHalfSize + (z * voxelSize)));
								Vertices.Add(FVector(voxelHalfSize + (x * voxelSize), voxelHalfSize + (y * voxelSize), voxelHalfSize + (z * voxelSize)));

								Normals.Append(bNormals4, ARRAY_COUNT(bNormals4));
								break;
							}
							case 5:
							{
								Vertices.Add(FVector(-voxelHalfSize + (x * voxelSize), voxelHalfSize + (y * voxelSize), voxelHalfSize + (z * voxelSize)));
								Vertices.Add(FVector(-voxelHalfSize + (x * voxelSize), voxelHalfSize + (y * voxelSize), -voxelHalfSize + (z * voxelSize)));
								Vertices.Add(FVector(-voxelHalfSize + (x * voxelSize), -voxelHalfSize + (y * voxelSize), -voxelHalfSize + (z * voxelSize)));
								Vertices.Add(FVector(-voxelHalfSize + (x * voxelSize), -voxelHalfSize + (y * voxelSize), voxelHalfSize + (z * voxelSize)));

								Normals.Append(bNormals5, ARRAY_COUNT(bNormals5));
								break;
							}
							default:
								break;
							}

							UV.Append(bUVs, ARRAY_COUNT(bUVs));

							auto density = CalcDensity(x, y, z);
							// color.R - VoxelType, color.G - DestroyVoxelStage
							int32 DestroyStageMatIndex = 0;
							if (index == CurrentDestroyVoxelIndex)
							{
								DestroyStageMatIndex = DestroyStage / 10.f;
							}
							FColor color(FColor(chunkElements[index] - (static_cast<float>(DestroyStageMatIndex) * 100), DestroyStageMatIndex, 255, i));

							for (int j = 0; j < 4; ++j)
							{
								VertexColors.Add(color);
							}
						}

					}
					elementID += triangleVerticeNum;
				}
			}
		}
	}

	VoxelMeshComponent->ClearAllMeshSections();

	for (int SectionIndex = 0; SectionIndex < ChunkSection.Num(); ++SectionIndex)
	{
		auto& c = ChunkSection[SectionIndex];

		if (c.Vertices.Num() > 0)
		{
			VoxelMeshComponent->CreateMeshSection(SectionIndex, c.Vertices, c.Triangles, c.Normals, c.UV, c.VertexColors, c.Tangents, true);
		}
	}

	for (int MaterialIndex = 0; MaterialIndex < VoxelMaterials.Num(); ++MaterialIndex)
	{
		VoxelMeshComponent->SetMaterial(MaterialIndex, VoxelMaterials[MaterialIndex]);
	}
}

/*
 * Set Voxel
 * @param VoxelLocation - Voxel Location
 * @param value			- Set Voxel Type / Return Voxel original type
 */
void AVoxelChunk::SetVoxel(const FVector& VoxelLocation, EVoxelType& value)
{
	// Round off
	FVector LocalVoxelLocation = VoxelLocation + voxelHalfSize * FVector::OneVector;
	int32 x = LocalVoxelLocation.X / voxelSize;
	int32 y = LocalVoxelLocation.Y / voxelSize;
	int32 z = LocalVoxelLocation.Z / voxelSize;

	int32 index = x + (y * chunkXYSize) + (z * chunkXYSizeX2);
	EVoxelType ret = static_cast<EVoxelType>(chunkElements[index]);
	chunkElements[index] = static_cast<int32>(value);
	value = ret;

	UpdateMesh();
}

/*
 * DestroyVoxel
 */
bool AVoxelChunk::DestroyVoxel(const FVector& VoxelLocation, EVoxelType& e, float Value)
{
	// Round off
	FVector LocalVoxelLocation = VoxelLocation + voxelHalfSize * FVector::OneVector;
	int32 x = LocalVoxelLocation.X / voxelSize;
	int32 y = LocalVoxelLocation.Y / voxelSize;
	int32 z = LocalVoxelLocation.Z / voxelSize;

	int32 index = x + (y * chunkXYSize) + (z * chunkXYSizeX2);

	// Change Destroy Voxel
	if (index != CurrentDestroyVoxelIndex)
	{
		//if (CurrentDestroyVoxelIndex != -1)
		//{
		//	// Clear Stage
		//	chunkElements[CurrentDestroyVoxelIndex] %= 100;
		//}

		DestroyStage = 0.f;
		CurrentDestroyVoxelIndex = index;
	}

	// Stage - 100 200 300 ...
	// Voxel Type - 0 1 2 3 4 ..
	uint8 stage = DestroyStage / 10.f;

	// Destroy Final Stage
	if (DestroyStage >= MaxDestroyValue)
	{
		InitDestroyVoxel();

		EVoxelType ret = EVoxelType::Empty;
		SetVoxel(VoxelLocation, ret);

		if (ret != EVoxelType::Empty)
		{
			e = ret;
			return true;
		}
	}

	UpdateMesh();

	DestroyStage += Value * DestroySpeed;
	return false;
}

void AVoxelChunk::InitDestroyVoxel()
{
	//if (CurrentDestroyVoxelIndex >= 0)
	//{
	//	chunkElements[CurrentDestroyVoxelIndex] %= 100;
	//}
	CurrentDestroyVoxelIndex = -1;
	CurrentDestroyVoxelType = EVoxelType::Empty;
	DestroyStage = 0.f;

	UpdateMesh();
}

// Called when the game starts or when spawned
void AVoxelChunk::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AVoxelChunk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

