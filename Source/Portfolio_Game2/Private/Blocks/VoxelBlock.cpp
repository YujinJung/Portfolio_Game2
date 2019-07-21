// Fill out your copyright notice in the Description page of Project Settings.


#include "VoxelBlock.h"
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
const FVector bNormals2[] = { FVector(0, 1, 0), FVector(0, 1, 0), FVector(0, 1, 0), FVector(0, 1, 0)};		

// left
const FVector bNormals3[] = { FVector(0, -1, 0), FVector(0, -1, 0), FVector(0, -1, 0), FVector(0, -1, 0)};	

// front
const FVector bNormals4[] = { FVector(1, 0, 0), FVector(1, 0, 0), FVector(1, 0, 0), FVector(1, 0, 0) };		

// back
const FVector bNormals5[] = { FVector(-1, 0, 0), FVector(-1, 0, 0), FVector(-1, 0, 0), FVector(-1, 0, 0) }; 
const FVector bMask[] = { FVector(0.000000, 0.000000, 1.000000),FVector(0.000000, 0.000000, -1.000000),FVector(0.000000, 1.000000, 0.000000),FVector(0.000000, -1.000000, 0.000000), FVector(1.000000, 0.000000, 0.000000), FVector(-1.000000, 0.000000, 0.000000) };


// Sets default values
AVoxelBlock::AVoxelBlock()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	chunkZSize = 40;
	chunkXYSize = 20;
	chunkXYSizeX2 = chunkXYSize * chunkXYSize;
	chunkTotalSize = chunkXYSize * chunkXYSize * chunkZSize;

	FString name = "Voxel_" + FString::FromInt(chunkXIndex) + "_" + FString::FromInt(chunkYIndex);
	BlockMeshComponent = NewObject<UProceduralMeshComponent>(this, *name);
	BlockMeshComponent->RegisterComponent();

	RootComponent = BlockMeshComponent;

	voxelHalfSize = voxelSize / 2.f;

	FString MaterialPath(TEXT("/Game/MinecraftContents/Materials/Block/M_Grass"));
	BlockMaterial = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, *MaterialPath));
}

float AVoxelBlock::CalcDensity(float x, float y, float z)
{
	const float cliffScale = 7.f;
	const float noiseScale = 3.f;
	const float offset = 5.f;
	float noise = USimplexNoiseBPLibrary::SimplexNoise3D(x * 0.02f, y* 0.02f, z*0.01f);

	float cliff = (noise * 0.5f + 0.5f) * cliffScale;
	float density = (noise + cliff) * noiseScale + offset - z;
	//UE_LOG(LogTemp, Warning, TEXT("Noise : %.4f"), noise);
	return density;
}

void AVoxelBlock::GenerateChunk(const FVector& ChunkLocation)
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

void AVoxelBlock::UpdateMesh()
{
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UV;
	TArray<FColor> VertexColors;
	TArray<FProcMeshTangent> Tangents;

	int32 elementID = 0;

	for (int32 x = 0; x < chunkXYSize; ++x)
	{
		for (int32 y = 0; y < chunkXYSize; ++y)
		{
			for (int32 z = 0; z < chunkZSize; ++z)
			{
				int32 index = x + (y * chunkXYSize) + (z * chunkXYSizeX2);

				if (chunkElements[index] > 0)
				{
					int triangleVerticeNum = 0;
					for (int i = 0; i < 6; ++i)
					{
						// next or previous block for each side
						int newIndex = index + bMask[i].X + (bMask[i].Y * chunkXYSize) + (bMask[i].Z * chunkXYSizeX2);
						
						bool flag = true;

						FVector newIndexVector(x, y, z);
						newIndexVector += bMask[i];

						if ((newIndexVector.X < chunkXYSize) && (newIndexVector.X >= 0) &&
							(newIndexVector.Y < chunkXYSize) && (newIndexVector.Y >= 0) &&
							(newIndexVector.Z < chunkZSize) && (newIndexVector.Z >= 0))
						{
							if (newIndex < chunkElements.Num() && newIndex >= 0)
								if(chunkElements[newIndex] > 0)
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
							FColor color(FColor(255, 255, 255, i));
							
							if(chunkElements[index] == 1 /* && check grass block */)
							{
								color.A = 1;
							}

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

	BlockMeshComponent->ClearAllMeshSections();

	BlockMeshComponent->CreateMeshSection(0, Vertices, Triangles, Normals, UV, VertexColors, Tangents, true);

	if (BlockMaterial)
	{
		BlockMeshComponent->SetMaterial(0, BlockMaterial);
	}
}

void AVoxelBlock::SetVoxel(FVector VoxelPos, EVoxelType value)
{
	// Round off
	VoxelPos += FVector(voxelHalfSize, voxelHalfSize, voxelHalfSize);
	int32 x = VoxelPos.X / voxelSize;
	int32 y = VoxelPos.Y / voxelSize;
	int32 z = VoxelPos.Z / voxelSize;

	int32 index = x + (y * chunkXYSize) + (z * chunkXYSizeX2);
	chunkElements[index] = static_cast<int32>(value);

	UpdateMesh();
}

// Called when the game starts or when spawned
void AVoxelBlock::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AVoxelBlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

