// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VoxelBlock.generated.h"

UENUM()
enum class VoxelType : uint8 {
	Empty,
	Dirt,
	Grass,
	Rock,
};

UCLASS()
class PORTFOLIO_GAME2_API AVoxelBlock : public AActor
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Sets default values for this actor's properties
	AVoxelBlock();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	float CalcDensity(float x, float y, float z);

	void GenerateChunk(const FVector& PlayerLocation);

	void UpdateMesh();

	FORCEINLINE const int32 GetChunkXYSize() const { return chunkXYSize; }
	
	void SetVoxel(FVector VoxelPos, VoxelType value);

public:
	UPROPERTY()
	class UProceduralMeshComponent* BlockMeshComponent;

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
	UPROPERTY()
	int32 voxelSize = 100;
	UPROPERTY()
	int32 voxelHalfSize;
	
	UPROPERTY()
	TArray<int32> chunkElements;

private:
	class UMaterial* BlockMaterial;

};
