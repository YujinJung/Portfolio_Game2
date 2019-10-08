// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TreeVoxel.generated.h"

UCLASS()
class PORTFOLIO_GAME2_API ATreeVoxel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATreeVoxel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void AddVoxelMaterial(FString MaterialName);

	void GenerateVoxel();

private:
	TArray<class UMaterial*> VoxelMaterials;
};
