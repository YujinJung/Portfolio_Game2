// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Portfolio_Game2_Common.h"
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

	UFUNCTION(BlueprintCallable, Category = Voxel)
	void GenerateVoxel(const FVector& VoxelLocation, EVoxelType e);
	UFUNCTION(BlueprintCallable, Category = Voxel)
	void CheckGravity();

private:
	UPROPERTY()
	class UProceduralMeshComponent* VoxelMeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Voxel, meta = (AllowPrivateAccess = true))
	class UMaterial* VoxelMaterials;

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
