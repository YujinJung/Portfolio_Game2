// Fill out your copyright notice in the Description page of Project Settings.


#include "DestroyedVoxel.h"
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

// Sets default values
ADestroyedVoxel::ADestroyedVoxel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FString name = "Voxel_Destroyed";
	VoxelMeshComponent = NewObject<UProceduralMeshComponent>(this, *name);
	VoxelMeshComponent->RegisterComponent();
	RootComponent = VoxelMeshComponent;

	voxelSize = 30;
	voxelHalfSize = voxelSize / 2;

	FString MaterialPath(TEXT("/Game/MinecraftContents/Materials/Voxels/M_Voxel"));
	VoxelMaterials = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, *MaterialPath));
}

// Called every frame
void ADestroyedVoxel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADestroyedVoxel::GenerateVoxel(const FVector& VoxelLocation, EVoxelType e)
{
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UV;
	TArray<FColor> VertexColors;
	TArray<FProcMeshTangent> Tangents;

	int32 triangleVerticeNum = 0;

	const float& x = VoxelLocation.X;
	const float& y = VoxelLocation.Y;
	const float& z = VoxelLocation.Z;
	UE_LOG(LogTemp, Warning, TEXT("xyz (%.3f, %.3f, %.3f), %d"), x, y, z, static_cast<int>(e));
	/*for (int i = 0; i < 6; ++i)
	{
			for (int j = 0; j < 6; ++j)
			{
				Triangles.Add(bTriangles[j] + triangleVerticeNum);
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

			// red = 1 -> Grass
			FColor color(FColor(static_cast<int>(e), 255, 255, i));

			for (int j = 0; j < 4; ++j)
			{
				VertexColors.Add(color);
			}
	}*/

	for (int i = 0; i < 6; ++i)
	{
			for (int j = 0; j < 6; ++j)
			{
				Triangles.Add(bTriangles[j] + triangleVerticeNum);
			}
			triangleVerticeNum += 4;

			switch (i)
			{
			case 0:
			{
				Vertices.Add(FVector(voxelHalfSize, 1.5f * voxelSize, 1.5f * voxelSize));
				Vertices.Add(FVector(voxelHalfSize, voxelHalfSize, 1.5f * voxelSize));
				Vertices.Add(FVector(1.5f * voxelSize, voxelHalfSize, 1.5f * voxelSize));
				Vertices.Add(FVector(1.5f * voxelSize, 1.5f * voxelSize, 1.5f * voxelSize));

				Normals.Append(bNormals0, ARRAY_COUNT(bNormals0));
				break;
			}
			case 1:
			{
				Vertices.Add(FVector(1.5f * voxelSize, voxelHalfSize, voxelHalfSize));
				Vertices.Add(FVector(voxelHalfSize, voxelHalfSize, voxelHalfSize));
				Vertices.Add(FVector(voxelHalfSize, 1.5f * voxelSize, voxelHalfSize));
				Vertices.Add(FVector(1.5f * voxelSize, 1.5f * voxelSize, voxelHalfSize));

				Normals.Append(bNormals1, ARRAY_COUNT(bNormals1));
				break;
			}
			case 2:
			{
				Vertices.Add(FVector(1.5f * voxelSize, 1.5f * voxelSize, 1.5f * voxelSize));
				Vertices.Add(FVector(1.5f * voxelSize, 1.5f * voxelSize, voxelHalfSize));
				Vertices.Add(FVector(voxelHalfSize, 1.5f * voxelSize, voxelHalfSize));
				Vertices.Add(FVector(voxelHalfSize, 1.5f * voxelSize, 1.5f * voxelSize));

				Normals.Append(bNormals2, ARRAY_COUNT(bNormals2));
				break;
			}
			case 3:
			{
				Vertices.Add(FVector(voxelHalfSize, voxelHalfSize, 1.5f * voxelSize));
				Vertices.Add(FVector(voxelHalfSize, voxelHalfSize, voxelHalfSize));
				Vertices.Add(FVector(1.5f * voxelSize, voxelHalfSize, voxelHalfSize));
				Vertices.Add(FVector(1.5f * voxelSize, voxelHalfSize, 1.5f * voxelSize));

				Normals.Append(bNormals3, ARRAY_COUNT(bNormals3));
				break;
			}
			case 4:
			{
				Vertices.Add(FVector(1.5f * voxelSize, voxelHalfSize, 1.5f * voxelSize));
				Vertices.Add(FVector(1.5f * voxelSize, voxelHalfSize, voxelHalfSize));
				Vertices.Add(FVector(1.5f * voxelSize, 1.5f * voxelSize, voxelHalfSize));
				Vertices.Add(FVector(1.5f * voxelSize, 1.5f * voxelSize, 1.5f * voxelSize));

				Normals.Append(bNormals4, ARRAY_COUNT(bNormals4));
				break;
			}
			case 5:
			{
				Vertices.Add(FVector(voxelHalfSize, 1.5f * voxelSize, 1.5f * voxelSize));
				Vertices.Add(FVector(voxelHalfSize, 1.5f * voxelSize, voxelHalfSize));
				Vertices.Add(FVector(voxelHalfSize, voxelHalfSize, voxelHalfSize));
				Vertices.Add(FVector(voxelHalfSize, voxelHalfSize, 1.5f * voxelSize));

				Normals.Append(bNormals5, ARRAY_COUNT(bNormals5));
				break;
			}
			default:
				break;
			}

			UV.Append(bUVs, ARRAY_COUNT(bUVs));

			// red = 1 -> Grass
			FColor color(FColor(static_cast<int>(e), 255, 255, i));

			for (int j = 0; j < 4; ++j)
			{
				VertexColors.Add(color);
			}
	}


	VoxelMeshComponent->ClearAllMeshSections();

	VoxelMeshComponent->CreateMeshSection(0, Vertices, Triangles, Normals, UV, VertexColors, Tangents, true);

	if (VoxelMaterials)
	{
		VoxelMeshComponent->SetMaterial(0, VoxelMaterials);
	}
}

