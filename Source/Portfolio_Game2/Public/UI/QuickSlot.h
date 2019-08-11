// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object.h"
#include "VoxelItemInfo.h"
#include "QuickSlot.generated.h"

/**
 * 
 */
UCLASS()
class PORTFOLIO_GAME2_API UQuickSlot : public UObject
{
	GENERATED_BODY()

public:
	UQuickSlot();

private:
		// Quick Slot
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = QuickSlot, meta = (AllowPrivateAccess = true))
		TArray<FVoxelItemInfo> QuickSlotVoxelItemArray;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = QuickSlot, meta = (AllowPrivateAccess = true))
		FVoxelItemInfo CurrentVoxelItem;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = QuickSlot, meta = (AllowPrivateAccess = true))
		uint8 MaxVoxelItemNum;


public:
	// Quick Slot / Inventory System
	UFUNCTION(BlueprintCallable, Category = Voxel)
		bool LootingVoxel(const FVoxelItemInfo& LootingVoxelItem);
	UFUNCTION(BlueprintCallable, Category = Voxel)
		bool isValidCurrentVoxelItem() const;

	UFUNCTION(BlueprintPure, Category = QuickSlot)
		uint8 GetQuickSlotEmptyIndex(const FVoxelItemInfo& LootingVoxel);
	UFUNCTION(BlueprintPure, Category = QuickSlot)
		FORCEINLINE FVoxelItemInfo GetCurrentVoxelItem() const { return CurrentVoxelItem; }
	UFUNCTION(BlueprintPure, Category = QuickSlot)
		FORCEINLINE TArray<FVoxelItemInfo> GetQuickSlotVoxelItemArray() const { return QuickSlotVoxelItemArray; }

	UFUNCTION(BlueprintCallable, Category = QuickSlot)
		void SetCurrentVoxelItem(const FVoxelItemInfo& VoxelInfo);
	UFUNCTION(BlueprintCallable, Category = QuickSlot)
		void SetCurrentVoxelItemWithIndex(int32 index);
	UFUNCTION(BlueprintCallable, Category = QuickSlot)
		void SetQuickSlotVoxelItemArray(EVoxelType inVoxelType, int32 num, int32 index);
};
