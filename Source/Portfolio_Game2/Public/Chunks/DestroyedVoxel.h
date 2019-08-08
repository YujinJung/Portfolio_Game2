// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VoxelItemInfo.h"
#include "DestroyedVoxel.generated.h"


UCLASS()
class PORTFOLIO_GAME2_API ADestroyedVoxel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADestroyedVoxel();

public:	
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = DestroyedVoxel)
	void GenerateVoxel(const FVector& VoxelLocation, EVoxelType e);
	UFUNCTION(BlueprintPure, Category = DestroyedVoxel)
	bool CheckLifeTime();
	UFUNCTION(BlueprintCallable, Category = DestroyedVoxel)
	void CheckGravity();
	UFUNCTION(BlueprintCallable, Category = DestroyedVoxel)
	void SetNum(int32 num);
	UFUNCTION(BlueprintPure, Category = DestroyedVoxel)
	FORCEINLINE FVector GetBaseLocation() const { return BaseLocation; }
	UFUNCTION(BlueprintPure, Category = DestroyedVoxel)
	FORCEINLINE EVoxelType GetVoxelItemType() { return VoxelInfo.VoxelType; }
	UFUNCTION(BlueprintPure, Category = DestroyedVoxel)
	FORCEINLINE int32 GetVoxelItemNum() { return VoxelInfo.Num; }
	
private:
	UPROPERTY()
	class UProceduralMeshComponent* VoxelMeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Voxel, meta = (AllowPrivateAccess = true))
	class UMaterial* VoxelMaterials;
	/*UPROPERTY()
	EVoxelType VoxelType;*/
	UPROPERTY()
	FVoxelItemInfo VoxelInfo;

	UPROPERTY()
	int32 voxelSize;
	UPROPERTY()
	int32 voxelHalfSize;
	UPROPERTY()
	int32 originalVoxelHalfSize;

	UPROPERTY()
	bool isDown;
	UPROPERTY()
	FVector BaseLocation;
	
	UPROPERTY()
	float sinScale;
	UPROPERTY()
	float DownScale;

	UPROPERTY()
	float RunningTime;
	UPROPERTY()
	float DownRunningTime; // Move Downward Time
};
