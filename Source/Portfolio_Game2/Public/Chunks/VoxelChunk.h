// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Portfolio_Game2_Common.h"
#include "ProceduralMeshComponent.h"
#include "VoxelChunk.generated.h"

USTRUCT(BlueprintType)
struct FVoxelIndexInSection
{
	GENERATED_USTRUCT_BODY();
public:
	FVoxelIndexInSection() {}
	FVoxelIndexInSection(int32 _voxelIndex, int32 _drawSide)
		: StartIndex(_voxelIndex), DrawSide(_drawSide)
	{ }

	/* Voxel index in section */
	int32 StartIndex = 0;
	/* The number of sides to draw in the voxel */
	int32 DrawSide = 0;
};

USTRUCT(BlueprintType)
struct FVoxelChunkSection
{
	GENERATED_USTRUCT_BODY();
public:
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UV;
	TArray<FColor> VertexColors;
	TArray<FProcMeshTangent> Tangents;

	TMap<int32, FVoxelIndexInSection> VertexIndex;

	int32 elementID = 0;
};


UCLASS()
class PORTFOLIO_GAME2_API AVoxelChunk : public AActor
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Sets default values for this actor's properties
	AVoxelChunk();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/* Add Voxel Material */
	UFUNCTION(BlueprintCallable, Category = Voxel)
	void AddVoxelMaterial(FString MaterialName);

	/* Use FindChunk By Index*/
	UFUNCTION()
	FORCEINLINE FVector2D GetChunkIndex() const { return ChunkIndex; }
	UFUNCTION()
	void SetChunkIndex(const FVector2D& _chunkIndex);

private:
	/* Use FindChunk By Index*/
	UPROPERTY()
	FVector2D ChunkIndex;

	UPROPERTY()
	TArray<class UMaterial*> VoxelMaterials;


/* Generate Chunk*/
public:
	/* Calculate Density By NoiseFunction*/
	UFUNCTION(BlueprintCallable, Category = Voxel)
	float CalcDensity(float x, float y, float z);

	/* Decide voxel type */
	UFUNCTION(BlueprintCallable, Category = Voxel)
	void GenerateVoxelType(const FVector& PlayerLocation);

	/* Generate Mesh */
	UFUNCTION(BlueprintCallable, Category = Voxel)
	void GenerateChunk();

	UFUNCTION()
	void RefreshMesh();


private:
	UPROPERTY()
	class UProceduralMeshComponent* VoxelMeshComponent;

	UPROPERTY()
	TArray<EVoxelType> chunkElements;

	/* Spawn Tree Random Seed */
	UPROPERTY()
	int32 TreeRandomSeed;

	FTimerHandle MapLoadTimerHandle;
	
	/* const */
	UPROPERTY()
	int32 voxelSize;

	UPROPERTY()
	int32 voxelHalfSize;

	UPROPERTY(EditAnywhere)
	int32 chunkZSize;

	UPROPERTY(EditAnywhere)
	int32 chunkXYSize;

	UPROPERTY()
	int32 chunkXYSizeX2;
	
	UPROPERTY()
	int32 chunkTotalSize;

	UPROPERTY()
	int32 chunkXIndex;

	UPROPERTY()
	int32 chunkYIndex;

	
/* Place and Destroy Voxel */
public:
	/* Set Voxel Type and return origin voxel type. Use Place Voxel*/
	UFUNCTION(BlueprintCallable, Category = Voxel)
	bool SetVoxel(const FVector& VoxelPos, EVoxelType& value);

	UFUNCTION(BlueprintCallable, Category = Voxel)
	bool DestroyVoxel(const FVector& VoxelLocation, EVoxelType& e, float Value);

	UFUNCTION(BlueprintCallable, Category = Voxel)
	void InitDestroyVoxel();

	void SetIsRunningTime(bool r);

private:
	/* Current ChunkSection INFO for Update */
	UPROPERTY()
	TArray<FVoxelChunkSection> ChunkSectionInfo;

	/* Destroy */
	UPROPERTY()
	int32 CurrentDestroyVoxelIndex; // -1 : no Destroy Voxel
	UPROPERTY()
	EVoxelType CurrentDestroyVoxelType;
	UPROPERTY()
	float DestroyStage;
	UPROPERTY()
	float DestroySpeed; // speed
	UPROPERTY()
	float MaxDestroyValue;


private:
	UPROPERTY()
	TMap<int32, float> chunkElementsTime; // index, time
	UPROPERTY()
	bool isRunningTime;
};

