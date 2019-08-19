// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Portfolio_Game2_Common.h"
#include "VoxelChunk.generated.h"



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

	float CalcDensity(float x, float y, float z);

	UFUNCTION(BlueprintCallable, Category = Voxel)
	void SetVoxelMaterial(FString MaterialPath);

	UFUNCTION(BlueprintCallable, Category = Voxel)
	void GenerateChunk(const FVector& PlayerLocation);

	UFUNCTION(BlueprintCallable, Category = Voxel)
	void UpdateMesh();

	UFUNCTION()
	void RefreshMesh();

	UFUNCTION(BlueprintCallable, Category = Voxel)
	void SetVoxel(const FVector& VoxelPos, EVoxelType& value);
	
	UFUNCTION(BlueprintCallable, Category = Voxel)
	bool DestroyVoxel(const FVector& VoxelLocation, EVoxelType& e, float Value);

	UFUNCTION(BlueprintCallable, Category = Voxel)
	void InitDestroyVoxel();

	FORCEINLINE const int32 GetChunkXYSize() const { return chunkXYSize; }

	void SetIsRunningTime(bool r);

private:
	UPROPERTY()
	class UProceduralMeshComponent* VoxelMeshComponent;

	FTimerHandle MapLoadTimerHandle;
	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Voxel, meta=(AllowPrivateAccess = true))
	class UMaterial* VoxelMaterials;
*/
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
	int32 voxelSize;
	UPROPERTY()
	int32 voxelHalfSize;
	
	UPROPERTY()
	TArray<EVoxelType> chunkElements;
	UPROPERTY()
	TMap<int32, float> chunkElementsTime; // index, time
	UPROPERTY()
	bool isRunningTime;

	UPROPERTY()
	TArray<class UMaterial*> VoxelMaterials;

	// Destroy
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
};
