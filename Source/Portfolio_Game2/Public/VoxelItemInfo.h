#pragma once

#include "Portfolio_Game2_Common.h"
#include "VoxelItemInfo.generated.h"

/*
 * Voxel Item Information
 */
USTRUCT(BlueprintType)
struct FVoxelItemInfo
{
	GENERATED_USTRUCT_BODY()

	/** The voxels you have */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TestStruct")
	int32 Num;
	/** Volxe Type by EVoxelType */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TestStruct")
	EVoxelType VoxelType;
	

public:
	FVoxelItemInfo()
		: Num(0), VoxelType(EVoxelType::Empty)
	{}

};
