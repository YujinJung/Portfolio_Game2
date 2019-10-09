// Fill out your copyright notice in the Description page of Project Settings.

#include "VoxelChunk.h"
#include "SimplexNoiseBPLibrary.h"
#include "TimerManager.h"
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

// top, bot, side ...
const FVector bMask[] = { FVector(0.000000, 0.000000, 1.000000),FVector(0.000000, 0.000000, -1.000000),FVector(0.000000, 1.000000, 0.000000),FVector(0.000000, -1.000000, 0.000000), FVector(1.000000, 0.000000, 0.000000), FVector(-1.000000, 0.000000, 0.000000) };


// Sets default values
AVoxelChunk::AVoxelChunk()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	FString name = "Voxel_" + FString::FromInt(chunkXIndex) + "_" + FString::FromInt(chunkYIndex);
	VoxelMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(*name);
	VoxelMeshComponent->RegisterComponent();
	RootComponent = VoxelMeshComponent;
	VoxelMeshComponent->bUseAsyncCooking = true;
	bIsCurrentChunk = false;

	voxelSize = 100;
	voxelHalfSize = voxelSize / 2;

	chunkXYSize = 18; // 16 + 2
	chunkZSize = 18; 
	chunkXYSizeX2 = chunkXYSize * chunkXYSize;
	chunkTotalSize = chunkXYSize * chunkXYSize * chunkZSize;

	DestroyStage = 0.f;
	CurrentDestroyVoxelIndex = -1;

	TreeRandomSeed = 10;

	MaxDestroyValue = 80.f;
	DestroySpeed = 0.1f;
	isRunningTime = false;
	bIsDestroyingTree = false;

	AddVoxelMaterial(TEXT("M_Dirt"));
	AddVoxelMaterial(TEXT("M_Grass"));
	AddVoxelMaterial(TEXT("M_Sand"));
	AddVoxelMaterial(TEXT("M_Stone"));
	AddVoxelMaterial(TEXT("M_Log"));
	AddVoxelMaterial(TEXT("M_Leaves"));
	AddVoxelMaterial(TEXT("M_Leaves_Far"));
}

void AVoxelChunk::AddVoxelMaterial(FString MaterialName)
{
	FString MaterialPath = TEXT("/Game/MinecraftContents/Materials/Voxels/");
	MaterialPath.Append(MaterialName);
	UMaterial* VoxelMaterial = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, *MaterialPath));

	if (VoxelMaterial)
	{
		VoxelMaterials.Add(VoxelMaterial);
	}
}

// Called when the game starts or when spawned
void AVoxelChunk::BeginPlay()
{
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(MapLoadTimerHandle, this, &AVoxelChunk::RefreshMesh, 3.f, true);
}

// Called every frame
void AVoxelChunk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AVoxelChunk::SetChunkIndex(const FIntVector& _chunkIndex)
{
	ChunkIndex = _chunkIndex;
	chunkXIndex = _chunkIndex.X;
	chunkYIndex = _chunkIndex.Y;
}

float AVoxelChunk::CalcDensity(float x, float y, float z)
{
	const float cliffScale = 7.f;
	const float noiseScale = 5.f;
	const float offset = 5.f;
	float noise = USimplexNoiseBPLibrary::SimplexNoise3D(x * 0.02f, y * 0.02f, z * 0.03f);

	float cliff = (noise * 0.5f + 0.5f) * cliffScale;
	float density = (noise + cliff) * noiseScale + offset - z;
	//UE_LOG(LogTemp, Warning, TEXT("Noise : %.4f"), noise);
	return density;
}

/*
 * Generate Voxel Type
 * Call only when spawned
 * @param ChunkLocation : chunk location index
 */
bool AVoxelChunk::GenerateVoxelType(const FVector& ChunkLocation)
{
	chunkElements.SetNumUninitialized(chunkTotalSize);
	FRandomStream RandomStream = FRandomStream(TreeRandomSeed);

	for (int32 x = 0; x < chunkXYSize; ++x)
	{
		for (int32 y = 0; y < chunkXYSize; ++y)
		{
			bool isTop = true; // check top 
			int airCount = 0;
			for (int32 z = chunkXYSize - 1; z >= 0; --z)
			{
				int32 index = x + (y * chunkXYSize) + (z * chunkXYSizeX2);

				auto densityCoord = [&chunkXYSize = chunkXYSize](const float& a, const int32& b) -> float { return (a * (chunkXYSize - 2) + b - 1); };
				float density = CalcDensity(densityCoord(ChunkLocation.X, x), densityCoord(ChunkLocation.Y, y), densityCoord(ChunkLocation.Z, z));
				
				/*if (density > 0.f)
				{
					float _z = densityCoord(ChunkLocation.Z, z);

					if (_z != 0)
					{
						density += 30.f * (1 / _z);
					}
				}*/
			
				if (density < 0.f)
				{
					chunkElements[index] = EVoxelType::Empty;
					airCount++;
					if (airCount == 3) { isTop = true; airCount = 0; }
				}
				/*else if ((density + ((1 - (z / chunkZSize)) * 4.5f)) >= 5.f)
				{
					chunkElements[index] = EVoxelType::Stone;
				}*/
				else if (density >= 5.0f)
				{
					chunkElements[index] = EVoxelType::Stone;
				}
				else if (density >= 0.f)
				{
					if (isTop)
					{
						static auto CheckEdge = [&chunkXYSize = chunkXYSize](const int& x) -> bool { return ((x == 0) || (x == chunkXYSize - 1)) ? false : true; };
						if (bIsTopChunk && CheckEdge(x) && CheckEdge(y) && RandomStream.FRand() < 0.01f)
						{
							TreeIndex.Add(FIntVector(x - 1, y - 1, z));
						}
						chunkElements[index] = EVoxelType::Grass;
						isTop = false;
					}
					else
					{
						chunkElements[index] = EVoxelType::Dirt;
					}
					airCount = 0;
				}
			}
		}
	}

	TArray<int32> Tree;
	{
		// Tree Type
		for (auto& e : TreeIndex)
		{
			for (int x = -2; x < 3; ++x)
			{
				for (int y = -2; y < 3; ++y)
				{
					for (int z = 0; z < 6; ++z)
					{
						const int tx = x + e.X;
						const int ty = y + e.Y;
						const int tz = z + e.Z;
						
						int32 index = tx + (ty * chunkXYSize) + (tz * chunkXYSizeX2);

						// Center
						if ((x == 0) && (y == 0) && (z < 5))
						{
							if (tz > 16 || index >= chunkElements.Num())
							{
								Tree.Add(index - ((tz - 16) * chunkXYSizeX2));
							}
							else
							{

							}

							TreeCoord.Add(FVoxelCoord(FIntVector(tx, ty, tz), EVoxelType::Log));
						}
						else if (z > 1)
						{
							if ((RandomStream.FRand() < 0.08f) || ((abs(x) < 2) && (abs(y) < 2)))
							{
								TreeCoord.Add(FVoxelCoord(FIntVector(tx, ty, tz), EVoxelType::Leaves_Far));
							}
						}
					}
				}
			}
		}
	}

	return GenerateChunk();
}

void AVoxelChunk::GenerateTree(TArray<FVoxelChunkSection>& ChunkSection)
{
	int i = 0;
	for (auto& e : TreeCoord)
	{
		const int32& TreeMeshIndex = static_cast<int32>(e.VoxelType) - 1;
		if (TreeMeshIndex >= 0)
		{
			auto& elementID		= ChunkSection[TreeMeshIndex].elementID;
			auto& VertexIndex	= ChunkSection[TreeMeshIndex].VertexIndex;
			auto& VertexColors	= ChunkSection[TreeMeshIndex].VertexColors;

			//int32 index = e.Coord.X + (e.Coord.Y * chunkXYSize) + (e.Coord.Z * chunkXYSizeX2);
			int32 index = chunkElements.Num() + i;

			int32 triangleVerticeNum = 0;
			for (int32 i = 0; i < 6; ++i)
			{
				DrawVoxels(ChunkSection, triangleVerticeNum, e.Coord, TreeMeshIndex, i);

				// color.R - DestroyVoxelStage, color.G - isChangeVoxel
				FColor color(0, 0, 0, i);
				// Destroy Stage
				if (index == CurrentDestroyVoxelIndex)
				{
					color.R = DestroyStage / 10.f;
				}
				
				for (int j = 0; j < 4; ++j)
				{
					VertexColors.Add(color);
				}
			}
			VertexIndex.Add(index, FVoxelIndexInSection(elementID, triangleVerticeNum));
			elementID += triangleVerticeNum;
		}
		++i;
	}
}

void AVoxelChunk::DrawVoxels(TArray<FVoxelChunkSection>& ChunkSection, int32& triangleVerticeNum, FIntVector VoxelIndex, const int32& VoxelMeshIndex, const int32& triangleSide)
{
	auto& elementID = ChunkSection[VoxelMeshIndex].elementID;
	auto& Vertices = ChunkSection[VoxelMeshIndex].Vertices;
	auto& Triangles = ChunkSection[VoxelMeshIndex].Triangles;
	auto& Normals = ChunkSection[VoxelMeshIndex].Normals;
	auto& UV = ChunkSection[VoxelMeshIndex].UV;
	auto& Tangents = ChunkSection[VoxelMeshIndex].Tangents;

	for (int j = 0; j < 6; ++j)
	{
		Triangles.Add(bTriangles[j] + triangleVerticeNum + elementID);
	}
	triangleVerticeNum += 4;

	/* vertex position
	 * ######
	 * #****#
	 * #****#
	 * ######
	 * * is current chunk, # is next chunk
	 * -1 is # (next chunk)
	 */

	int32& x = VoxelIndex.X;
	int32& y = VoxelIndex.Y;
	int32& z = VoxelIndex.Z;
	switch (triangleSide)
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
}

bool AVoxelChunk::GenerateChunk()
{
	bool ret = false;

	TArray<FVoxelChunkSection> ChunkSection;
	ChunkSection.SetNum(VoxelMaterials.Num());

	for (int32 x = 1; x < chunkXYSize - 1; ++x)
	{
		for (int32 y = 1; y < chunkXYSize - 1; ++y)
		{
			for (int32 z = 1; z < chunkZSize - 1; ++z)
			{
				int32 index = x + (y * chunkXYSize) + (z * chunkXYSizeX2);
				int32 VoxelMeshIndex = static_cast<int32>(chunkElements[index]);
				if (chunkElements[index] > EVoxelType::Count)
				{
					VoxelMeshIndex -= static_cast<int32>(EVoxelType::Count);
				}
				VoxelMeshIndex--; // EVoxelType::Empty

				if (VoxelMeshIndex >= 0)
				{
					auto& elementID		= ChunkSection[VoxelMeshIndex].elementID;
					auto& VertexIndex	= ChunkSection[VoxelMeshIndex].VertexIndex;
					auto& VertexColors	= ChunkSection[VoxelMeshIndex].VertexColors;

					int32 triangleVerticeNum = 0;
					for (int32 i = 0; i < 6; ++i)
					{
						bool flag = false;
						// check tree
						if (chunkElements[index] == EVoxelType::Leaves || chunkElements[index] == EVoxelType::Leaves_Far || chunkElements[index] == EVoxelType::Log)
						{
							flag = true;
						}

						// next or previous Voxel for each side
						if (!flag)
						{
							flag = true;

							int newIndex = index + bMask[i].X + (bMask[i].Y * chunkXYSize) + (bMask[i].Z * chunkXYSizeX2);

							FVector newIndexVector(x, y, z);
							newIndexVector += bMask[i];

							if ((newIndexVector.X < chunkXYSize) && (newIndexVector.X >= 0) &&
								(newIndexVector.Y < chunkXYSize) && (newIndexVector.Y >= 0) &&
								(newIndexVector.Z < chunkZSize) && (newIndexVector.Z >= 0))
							{
								if (newIndex < chunkElements.Num() && newIndex >= 0)
								{
									// exist side voxel
									if (chunkElements[newIndex] != EVoxelType::Empty)
									{
										flag = false;
									}
									else if ((i == 0) && (chunkElements[index] == EVoxelType::Dirt)) // no voxel top side, Grow grass
									{
										if (!chunkElementsTime.Contains(index))
										{
											chunkElementsTime.Add(index, 0.f);
										}
									}
								}
							}
						}

						if (flag)
						{
							ret = true;

							DrawVoxels(ChunkSection, triangleVerticeNum, FIntVector(x - 1, y - 1, z - 1), VoxelMeshIndex, i);

							// color.R - DestroyVoxelStage, color.G - isChangeVoxel
							FColor color(0, 0, 0, i);
							// Destroy Stage
							if (index == CurrentDestroyVoxelIndex)
							{
								color.R = DestroyStage / 10.f;
							}
							// Dirt->Grass, ...
							if (chunkElements[index] > EVoxelType::Count)
							{
								color.G = 2;
							}

							for (int j = 0; j < 4; ++j)
							{
								VertexColors.Add(color);
							}
						}

					}
					VertexIndex.Add(index, FVoxelIndexInSection(elementID, triangleVerticeNum));
					elementID += triangleVerticeNum;
				}
			}
		}
	}

	GenerateTree(ChunkSection);

	VoxelMeshComponent->ClearAllMeshSections();

	for (int SectionIndex = 0; SectionIndex < ChunkSection.Num(); ++SectionIndex)
	{
		auto& c = ChunkSection[SectionIndex];

		if (c.Vertices.Num() > 0)
		{
			VoxelMeshComponent->CreateMeshSection(SectionIndex, c.Vertices, c.Triangles, c.Normals, c.UV, c.VertexColors, c.Tangents, true);
		}
	}

	ChunkSectionInfo = ChunkSection;

	for (int MaterialIndex = 0; MaterialIndex < VoxelMaterials.Num(); ++MaterialIndex)
	{
		VoxelMeshComponent->SetMaterial(MaterialIndex, VoxelMaterials[MaterialIndex]);
	}

	return ret;
}

/*
 * Refresh Leaves Material 
 */
void AVoxelChunk::RefreshLeaves()
{
	for (auto& e : TreeCoord)
	{
		if (bIsCurrentChunk && (e.VoxelType == EVoxelType::Leaves_Far))
		{
			e.VoxelType = EVoxelType::Leaves;
		}
		else if (!bIsCurrentChunk && (e.VoxelType == EVoxelType::Leaves))
		{
			e.VoxelType = EVoxelType::Leaves_Far;
		}
	}

	GenerateChunk();
}

/*
 * Refresh Mesh
 * Grow Grass, ...
 */
void AVoxelChunk::RefreshMesh()
{
	//bool isChange = false;

	//for(auto& e : chunkElementsTime)
	//{
	//	if ((chunkElements[e.Key] == EVoxelType::Dirt) && (chunkElements[e.Key] == EVoxelType::Dirt_Grass))
	//	{
	//		if (e.Value > 5.0f)
	//		{
	//			chunkElements[e.Key] = EVoxelType::Grass;
	//			chunkElementsTime.Remove(e.Key);
	//			isChange = true;
	//			continue;
	//		}
	//		else if (e.Value > 3.0f)
	//		{
	//			chunkElements[e.Key] = EVoxelType::Dirt_Grass;
	//			isChange = true;
	//		}

	//		e.Value += Value;
	//	}
	//}

	//if (isChange)
	//{
	//	UpdateMesh();
	//}
}

/*
 * Set Voxel
 * @param VoxelLocation - Voxel Location
 * @param value			- Set Voxel Type / Return Voxel original type
 */
bool AVoxelChunk::SetVoxel(const FIntVector& VoxelLocation, EVoxelType& value, bool bIsDestroying)
{
	int32 index = VoxelLocation.X + (VoxelLocation.Y * chunkXYSize) + (VoxelLocation.Z * chunkXYSizeX2);

	// Range Check
	if ((index >= 0) && (index < chunkElements.Num()))
	{
		EVoxelType ret = chunkElements[index];
		if (!bIsDestroying && ret != EVoxelType::Empty) { return false; }

		if (value >= EVoxelType::Log && value <= EVoxelType::Leaves_Far)
		{
			TreeCoord.Add(FVoxelCoord(VoxelLocation - FIntVector(FVector::OneVector), value));
			chunkElements[index] = EVoxelType::Empty;
		}
		else
		{
			if (chunkElements[index] == EVoxelType::Empty)
			{
				return false;
			}
			chunkElements[index] = value;
		}

		value = ret;
		GenerateChunk();
		return true;
	}
	

	return false;
}

/*
 * DestroyVoxel
 */
bool AVoxelChunk::DestroyVoxel(const FIntVector& VoxelLocation, EVoxelType& e, float Value)
{
	// Round off
	int32 DestroyVoxelIndex = VoxelLocation.X + (VoxelLocation.Y * chunkXYSize) + (VoxelLocation.Z * chunkXYSizeX2);

	auto FindTreeIndex = [&TreeCoord = TreeCoord, &VoxelLocation = VoxelLocation, &chunkElements = chunkElements](int32& _D) -> bool {
		FIntVector TreeLocation = VoxelLocation - FIntVector(FVector::OneVector);
		for (int32 i = 0; i < TreeCoord.Num(); ++i)
		{
			if (TreeCoord[i].Coord == TreeLocation)
			{
				_D = chunkElements.Num() + i;
				return true;
			}
		}
		return false;
	};

	if (DestroyVoxelIndex >= 0 && DestroyVoxelIndex < chunkElements.Num())
	{
		if (chunkElements[DestroyVoxelIndex] != EVoxelType::Empty   &&
			chunkElements[DestroyVoxelIndex] != EVoxelType::Log     &&
			chunkElements[DestroyVoxelIndex] != EVoxelType::Leaves  &&
			chunkElements[DestroyVoxelIndex] != EVoxelType::Leaves_Far)
		{
			bIsDestroyingTree = false;
		}
		else
		{
			if (!FindTreeIndex(DestroyVoxelIndex))
			{
				return false;
			}
			bIsDestroyingTree = true;
		}
	}
	else
	{
		if (!FindTreeIndex(DestroyVoxelIndex))
		{
			return false;
		}
		bIsDestroyingTree = true;
	}

	// Change Destroy Voxel
	if (DestroyVoxelIndex != CurrentDestroyVoxelIndex)
	{
		DestroyStage = 0.f;

		CurrentDestroyVoxelIndex = DestroyVoxelIndex;
	}

	// Stage - 100 200 300 ...
	// Voxel Type - 0 1 2 3 4 ..
	uint8 stage = DestroyStage / 10.f;

	// Destroy Final Stage
	if (DestroyStage >= MaxDestroyValue)
	{
		InitDestroyVoxel();

		if (!bIsDestroyingTree)
		{
			e = chunkElements[DestroyVoxelIndex];
			chunkElements[DestroyVoxelIndex] = EVoxelType::Empty;
		}
		else
		{
			e = TreeCoord[DestroyVoxelIndex - chunkElements.Num()].VoxelType;
			TreeCoord.RemoveAtSwap(DestroyVoxelIndex - chunkElements.Num());
		}

		GenerateChunk();

		return true;
	}
	else // Destroy 1, 2, 3, ... stage | Do not destroy voxel, only change destroy effect(vertex color)
	{
		int32 SectionIndex;

		if (!bIsDestroyingTree)
		{
			// Section Index
			SectionIndex = static_cast<int32>(chunkElements[DestroyVoxelIndex]) - 1;
		}
		else
		{
			SectionIndex = static_cast<int32>(TreeCoord[DestroyVoxelIndex - chunkElements.Num()].VoxelType) - 1;
		}

		if (SectionIndex < 0) { return false; } // Voxel is Empty
		FVoxelChunkSection& ChunkSection = ChunkSectionInfo[SectionIndex];

		if (!ChunkSection.VertexIndex.Contains(DestroyVoxelIndex)) { return false; }
		int32 VertexIndex = ChunkSection.VertexIndex[DestroyVoxelIndex].StartIndex;
		int32 VoxelDrawSide = ChunkSection.VertexIndex[DestroyVoxelIndex].DrawSide;

		// For Update Mesh Section
		auto& VertexColors = ChunkSectionInfo[SectionIndex].VertexColors;
		for (int ChangeVoxelIndex = VertexIndex; ChangeVoxelIndex < VertexIndex + VoxelDrawSide; ++ChangeVoxelIndex)
		{
			if (ChangeVoxelIndex < VertexColors.Num())
			{
				VertexColors[ChangeVoxelIndex].R = DestroyStage / 10.f;
			}
		}

		VoxelMeshComponent->UpdateMeshSection(SectionIndex,
			ChunkSectionInfo[SectionIndex].Vertices,
			ChunkSectionInfo[SectionIndex].Normals,
			ChunkSectionInfo[SectionIndex].UV,
			VertexColors,
			ChunkSectionInfo[SectionIndex].Tangents);
	}

	DestroyStage += Value * DestroySpeed;
	return false;
}

void AVoxelChunk::InitDestroyVoxel()
{
	CurrentDestroyVoxelIndex = -1;
	CurrentDestroyVoxelType = EVoxelType::Empty;
	DestroyStage = 0.f;

	GenerateChunk();
}

void AVoxelChunk::SetIsRunningTime(bool r)
{
	isRunningTime = r;
}

